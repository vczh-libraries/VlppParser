#include <string.h>
#include "Common.h"

using namespace vl;
using namespace vl::stream;
using namespace vl::collections;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::definitions;
using namespace vl::parsing::analyzing;
using namespace vl::parsing::tabling;
using namespace vl::filesystem;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

namespace test
{
	template<typename TLoggable>
	void LogParsingData(TLoggable loggable, const Folder& folder, const WString& fileName, const WString& name, List<Ptr<ParsingError>>& errors = *(List<Ptr<ParsingError>>*)0)
	{
		TEST_PRINT(L"Writing " + fileName + L" ...");
		FileStream fileStream((folder.GetFilePath() / fileName).GetFullPath(), FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);

		writer.WriteLine(L"=============================================================");
		writer.WriteLine(name);
		writer.WriteLine(L"=============================================================");
		Log(loggable, writer);

		auto perrors = &errors; // clang++: -WUndefined-bool-conversion
		if (perrors && errors.Count() > 0)
		{
			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Errors");
			writer.WriteLine(L"=============================================================");
			FOREACH(Ptr<ParsingError>, error, errors)
			{
				writer.WriteLine(error->errorMessage);
			}
			TEST_PRINT(L"Errors are logged in \"" + fileName + L"\".");
		}
	}

	Ptr<ParsingTable> CreateTable(Ptr<ParsingDefinition> definition, const WString& name, bool enableAmbiguity)
	{
		Ptr<ParsingTable> deserializedTable;
		TEST_CASE(L"Create table: " + name)
		{
			TEST_PRINT(L"Building parser <" + name + L">");
			Folder outputFolder(GetTestOutputPath() + L"../Resources/Parsers/" + name);
			if (!outputFolder.Exists())
			{
				TEST_ASSERT(outputFolder.Create(false));
			}

			ParsingSymbolManager symbolManager;
			List<Ptr<ParsingError>> errors;
			ValidateDefinition(definition, &symbolManager, errors);
			LogParsingData(definition, outputFolder, L"Parsing." + name + L".Definition.txt", L"Grammar Definition", errors);
			TEST_ASSERT(errors.Count() == 0);

			Ptr<Automaton> epsilonPDA = CreateEpsilonPDA(definition, &symbolManager);
			Ptr<Automaton> nondeterministicPDA = CreateNondeterministicPDAFromEpsilonPDA(epsilonPDA);
			Ptr<Automaton> jointPDA = CreateJointPDAFromNondeterministicPDA(nondeterministicPDA);

			LogParsingData(epsilonPDA, outputFolder, L"Parsing." + name + L".EPDA.txt", L"Epsilon PDA");
			LogParsingData(nondeterministicPDA, outputFolder, L"Parsing." + name + L".NPDA.txt", L"Non-deterministic PDA");
			LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA.txt", L"Joint PDA");

			CompactJointPDA(jointPDA);
			LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA-Compacted.txt", L"Compacted Joint PDA");

			MarkLeftRecursiveInJointPDA(jointPDA, errors);
			LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA-Marked.txt", L"Compacted Joint PDA", errors);
			TEST_ASSERT(errors.Count() == 0);

			Ptr<ParsingTable> table = GenerateTableFromPDA(definition, &symbolManager, jointPDA, enableAmbiguity, errors);
			LogParsingData(table, outputFolder, L"Parsing." + name + L".Table.txt", L"Table", errors);
			if (!enableAmbiguity)
			{
				TEST_ASSERT(errors.Count() == 0);
			}

			TEST_PRINT(L"Serializing ...");
			MemoryStream stream;
			table->Serialize(stream);
			stream.SeekFromBegin(0);

			TEST_PRINT(L"Deserializing ...");
			deserializedTable = new ParsingTable(stream);
			TEST_ASSERT(stream.Position() == stream.Size());

			TEST_PRINT(L"Initializing ...");
			deserializedTable->Initialize();
		});
		return deserializedTable;
	}

	enum TokenStreamStatus
	{
		Normal,
		ResolvingAmbiguity,
		Closed,
	};

	Ptr<ParsingTreeNode> Parse(Ptr<ParsingTable> table, const WString& input, const WString& name, const WString& rule, vint index, bool showInput, bool autoRecover)
	{
		WString inputFirstLine;
		{
			StringReader reader(input);
			inputFirstLine = reader.ReadLine();
			if (input != inputFirstLine)
			{
				inputFirstLine += L" ...";
			}
		}

		Ptr<ParsingTreeNode> node;
		TEST_CASE(L"Parse: " + inputFirstLine)
		{
			TokenStreamStatus status = Normal;
			{
				FileStream fileStream(GetTestOutputPath() + L"Parsing." + name + L".[" + itow(index) + L"].txt", FileStream::WriteOnly);
				BomEncoder encoder(BomEncoder::Utf8);
				EncoderStream encoderStream(fileStream, encoder);
				StreamWriter writer(encoderStream);

				Ptr<ParsingGeneralParser> parser = autoRecover ? CreateAutoRecoverParser(table) : CreateStrictParser(table);
				parser->BeginParse();
				ParsingState state(input, table);
				List<Ptr<ParsingError>> errors;
				ParsingTreeBuilder builder;
				builder.Reset();

				writer.WriteLine(L"=============================================================");
				writer.WriteLine(L"Input");
				writer.WriteLine(L"=============================================================");
				writer.WriteLine(input);

				writer.WriteLine(L"=============================================================");
				writer.WriteLine(L"Tokens");
				writer.WriteLine(L"=============================================================");
				{
					FOREACH(RegexToken, token, state.GetTokens())
					{
						WString tokenName;
						TEST_ASSERT(token.token != -1);
						if (table->IsInputToken(token.token))
						{
							tokenName = table->GetTokenInfo(table->GetTableTokenIndex(token.token)).name;
						}
						else
						{
							tokenName = table->GetDiscardTokenInfo(table->GetTableDiscardTokenIndex(token.token)).name;
						}
						writer.WriteLine(tokenName + L": " + WString(token.reading, token.length));
					}
				}

				writer.WriteLine(L"=============================================================");
				writer.WriteLine(L"Transition");
				writer.WriteLine(L"=============================================================");
				vint startState = state.Reset(rule);
				TEST_ASSERT(startState != -1);
				writer.WriteLine(L"StartState: " + itow(startState) + L"[" + table->GetStateInfo(startState).stateName + L"]");

				while (true)
				{
					ParsingState::TransitionResult result = parser->ParseStep(state, errors);
					if (result)
					{
						switch (result.transitionType)
						{
						case ParsingState::TransitionResult::AmbiguityBegin:
							TEST_ASSERT(status == Normal);
							status = ResolvingAmbiguity;
							writer.WriteLine(L"<AmbiguityBegin> AffectedStackNodeCount=" + itow(result.ambiguityAffectedStackNodeCount));
							break;
						case ParsingState::TransitionResult::AmbiguityBranch:
							TEST_ASSERT(status == ResolvingAmbiguity);
							writer.WriteLine(L"<AmbiguityBranch>");
							break;
						case ParsingState::TransitionResult::AmbiguityEnd:
							TEST_ASSERT(status == ResolvingAmbiguity);
							status = Normal;
							writer.WriteLine(L"<AmbiguityEnd> AmbiguityNodeType=" + result.ambiguityNodeType);
							break;
						case ParsingState::TransitionResult::ExecuteInstructions:
							{
								writer.WriteLine(L"<ExecuteInstructions>");
								switch (result.tableTokenIndex)
								{
								case ParsingTable::TokenBegin:
									writer.WriteString(L"$TokenBegin => ");
									break;
								case ParsingTable::TokenFinish:
									writer.WriteString(L"$TokenFinish => ");
									TEST_ASSERT(status != Closed);
									if (status == Normal)
									{
										status = Closed;
									}
									break;
								case ParsingTable::NormalReduce:
									writer.WriteString(L"NormalReduce => ");
									break;
								case ParsingTable::LeftRecursiveReduce:
									writer.WriteString(L"LeftRecursiveReduce => ");
									break;
								default:
									writer.WriteString(table->GetTokenInfo(result.tableTokenIndex).name);
									writer.WriteString(L"[");
									if (result.token)
									{
										writer.WriteString(WString(result.token->reading, result.token->length));
									}
									writer.WriteString(L"] => ");
								}
								writer.WriteLine(itow(result.tableStateTarget) + L"[" + table->GetStateInfo(result.tableStateTarget).stateName + L"]");

								writer.WriteString(L"    <STACK>:");
								FOREACH(vint, state, state.GetStateStack())
								{
									writer.WriteString(L" " + itow(state));
								}
								writer.WriteLine(L"");

								vint shiftReduceRangeIndex = 0;
								FOREACH(ParsingTable::Instruction, ins, From(result.transition->instructions).Skip(result.instructionBegin).Take(result.instructionCount))
								{
									switch (ins.instructionType)
									{
									case ParsingTable::Instruction::Create:
										writer.WriteLine(L"    Create " + ins.nameParameter);
										break;
									case ParsingTable::Instruction::Using:
										writer.WriteLine(L"    Using");
										break;
									case ParsingTable::Instruction::Assign:
										writer.WriteLine(L"    Assign " + ins.nameParameter);
										break;
									case ParsingTable::Instruction::Item:
										writer.WriteLine(L"    Item " + ins.nameParameter);
										break;
									case ParsingTable::Instruction::Setter:
										writer.WriteLine(L"    Setter " + ins.nameParameter + L" = " + ins.value);
										break;
									case ParsingTable::Instruction::Shift:
										writer.WriteLine(L"    Shift " + itow(ins.stateParameter) + L"[" + table->GetStateInfo(ins.stateParameter).ruleName + L"]");
										break;
									case ParsingTable::Instruction::Reduce:
										writer.WriteLine(L"    Reduce " + itow(ins.stateParameter) + L"[" + table->GetStateInfo(ins.stateParameter).ruleName + L"]");
										if (result.shiftReduceRanges && showInput)
										{
											ParsingState::ShiftReduceRange range = result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
											writer.WriteString(L"    [");
											if (range.shiftToken && range.reduceToken)
											{
												vint start = range.shiftToken->start;
												vint end = range.reduceToken->start + range.reduceToken->length;
												writer.WriteString(input.Sub(start, end - start));
											}
											writer.WriteLine(L"]");
										}
										break;
									case ParsingTable::Instruction::LeftRecursiveReduce:
										writer.WriteLine(L"    LR-Reduce " + itow(ins.stateParameter) + L"[" + table->GetStateInfo(ins.stateParameter).ruleName + L"]");
										break;
									}
								}

								if (result.tableTokenIndex == ParsingTable::TokenFinish && status == Normal)
								{
									writer.WriteLine(L"");
									if (result.shiftReduceRanges && showInput)
									{
										ParsingState::ShiftReduceRange range = result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
										writer.WriteString(L"[");
										if (range.shiftToken && range.reduceToken)
										{
											vint start = range.shiftToken->start;
											vint end = range.reduceToken->start + range.reduceToken->length;
											writer.WriteString(input.Sub(start, end - start));
										}
										writer.WriteLine(L"]");
									}
								}
							}
							break;
						case ParsingState::TransitionResult::SkipToken:
							writer.WriteLine(L"<SkipToken>");
							break;
						}
						writer.WriteLine(L"");

						if (result.transitionType == ParsingState::TransitionResult::SkipToken)
						{
							if (state.GetCurrentTableTokenIndex() == ParsingTable::TokenFinish)
							{
								encoderStream.Close();
								fileStream.Close();
								TEST_ASSERT(false);
							}
							else
							{
								state.SkipCurrentToken();
							}
						}
						else if (!builder.Run(result))
						{
							encoderStream.Close();
							fileStream.Close();
							TEST_ASSERT(false);
						}
					}
					else
					{
						break;
					}
				}

				writer.WriteLine(L"=============================================================");
				writer.WriteLine(L"Tree");
				writer.WriteLine(L"=============================================================");
				node = builder.GetNode();
				if (node)
				{
					WString originalInput;
					if (showInput)
					{
						originalInput = input;
					}
					Log(node.Obj(), originalInput, writer);
				}
			}
			TEST_ASSERT(status == Closed);
			TEST_ASSERT(node);
		});
		return node;
	}

	Ptr<ParsingDefinition> LoadDefinition(const WString& parserName, const WString& text)
	{
		static bool firstLoad = false;
		if (!firstLoad)
		{
			firstLoad = true;
			Ptr<ParsingTable> table = CreateTable(CreateParserDefinition(), L"Syngram");
		}

		Ptr<ParsingDefinition> definition;
		TEST_CASE(L"Load definition: " + parserName)
		{
			Ptr<ParsingGeneralParser> parser = CreateBootstrapStrictParser();
			TEST_ASSERT(parser);

			List<Ptr<ParsingError>> errors;
			Ptr<ParsingTreeNode> node = parser->Parse(text, L"ParserDecl", errors);
			TEST_ASSERT(errors.Count() == 0);

			definition = DeserializeDefinition(node);
		});
		return definition;
	}

	Ptr<ParsingDefinition> LoadDefinition(const WString& parserName)
	{
		WString text;
		{
			WString fileName = GetTestResourcePath() + L"/Parsers/" + parserName + L".txt";
			FileStream fileStream(fileName, FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			text = reader.ReadToEnd();
		}
		return LoadDefinition(parserName, text);
	}

	WString ParsingDefinitionToText(Ptr<ParsingDefinition> definition)
	{
		return GenerateToStream([&](StreamWriter& writer)
		{
			Log(definition, writer);
		});
	}

	void ParseWithAutoRecover(Ptr<ParsingDefinition> definition, const WString& name, const WString& rule, List<WString>& inputs, bool enableAmbiguity)
	{
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTable> table;
		TEST_CASE(L"Generate table: " + name)
		{
			table = GenerateTable(definition, enableAmbiguity, errors);
			TEST_ASSERT(table);
			LogParsingData(table, Folder(GetTestOutputPath()), L"Parsing.AutoRecover[" + name + L"].Table.txt", L"Table");
		});

		FOREACH_INDEXER(WString, input, index, inputs)
		{
			Parse(table, input, L"AutoRecover[" + name + L"]", rule, index, true, true);
		}
	}
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
