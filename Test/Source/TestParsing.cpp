#include "Common.h"

using namespace test;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

TEST_FILE
{
	TEST_CATEGORY(L"NameList")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"NameList");
		Ptr<ParsingTable> table = CreateTable(definition, L"NameList");
		const wchar_t* inputs[] =
		{
			L"vczh",
			L"vczh, genius",
			L"vczh, genius, programmer",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"NameList", L"NameList", i, true, false);
		}
	});

	TEST_CATEGORY(L"Calculator")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"Calculator");
		Ptr<ParsingTable> table = CreateTable(definition, L"Calculator");
		const wchar_t* inputs[] =
		{
			L"0",
			L"1+2",
			L"1*2+3*4",
			L"(1+2)*(3+4)",
			L"exp()-pow(cos(1.57*2), 4)",
			L"1+1+1+1+1+1",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"Calculator", L"Exp", i, true, false);
		}
	});

	TEST_CATEGORY(L"Statement")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"Statement");
		Ptr<ParsingTable> table = CreateTable(definition, L"Statement");
		const wchar_t* inputs[] =
		{
			L"a = b",
			L"return value",
			L"{}",
			L"{ a = b c = d return e }",
			L"if a < b then return a else return b",
			L"{ if a < b then return a else return b }",
			L"if a < b then if b < c then return c else return b",
			L"if a < b then if b < c then return c else return b else return a",
			L"if a > b then { a = b if c <= d then e = f g = h } else { if a != b then return a else return b }",
			L"if a < b then if a > b then if a <= b then if a >= b then if a == b then if a != b then return null",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"Statement", L"Stat", i, true, false);
		}
	});

	TEST_CATEGORY(L"NameSemicolonList")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"NameSemicolonList");
		Ptr<ParsingTable> table = CreateTable(definition, L"NameSemicolonList");
		const wchar_t* inputs[] =
		{
			L"nothong =",
			L"author = vczh ;",
			L"languages = cpp ; csharp ; ",
			L"languages = cpp ; csharp ; vbdotnet ; ides = visualstudio ; eclipse ; xcode ;",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"NameSemicolonList", L"NameTable", i, true, false);
		}
	});

	TEST_CATEGORY(L"WorkflowType")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"WorkflowType");
		Ptr<ParsingTable> table = CreateTable(definition, L"WorkflowType");
		const wchar_t* inputs[] =
		{
			L"int",
			L"func():void",
			L"func(int, int):int",
			L"int[]",
			L"int[string]",
			L"int{}",
			L"int^",
			L"int::Representation",
			L"func(int{}, func(int):double{}):double{}",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"WorkflowType", L"WorkflowType", i, true, false);
		}
	});

	TEST_CATEGORY(L"AmbiguousExpression")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"AmbiguousExpression");
		Ptr<ParsingTable> table = CreateTable(definition, L"AmbiguousExpression", true);
		const wchar_t* inputs[] =
		{
			L"a",
			L"a b",
			L"a b c",
			L"a.b",
			L"a.b.c",
			L"a<b",
			L"a>b",
			L"a<>",
			L"a<b>c",
			L"a<b>.c",
			L"a<b,c>.d",
			L"a<b,c<d>>.e",
			L"a<b,c d>.e",
			L"a<b,c<d>e>.f g",
		};
		for (vint i = 0; i < sizeof(inputs) / sizeof(*inputs); i++)
		{
			Parse(table, inputs[i], L"AmbiguousExpression", L"Exp", i, true, false);
		}
	});

	TEST_CATEGORY(L"Grammar")
	{
		Ptr<ParsingDefinition> definition;
		Ptr<ParsingDefinition> inputDefs[] =
		{
			LoadDefinition(L"NameList"),
			LoadDefinition(L"Calculator"),
			LoadDefinition(L"Statement"),
			definition = CreateParserDefinition(),
		};
		const wchar_t* inputTexts[][2] =
		{
			{L"Type",			L"token"},
			{L"Type",			L"Item"},
			{L"Type",			L"Item[]"},
			{L"Type",			L"Item.SubItem"},
			{L"Type",			L"Item[][][]"},
			{L"Type",			L"Item.SubItem[]"},
			{L"Type",			L"Item[].SubItem[].AnotherSubItem[]"},

			{L"TypeDecl",		L"enum Boolean{ True, False, }"},
			{L"TypeDecl",		L"class Vector{ int x; int y; }"},
			{L"TypeDecl",		L"class ComplexVector : SimpleVector{"
									L"enum Ordering{ Ordered, Unordered, }"
									L"class Item{ string name; Ordering ordering; }"
									L"Item x; Item y;"
								L"}"},

			{L"Grammar",		L"Factor"},
			{L"Grammar",		L"\"*\""},
			{L"Grammar",		L"Factor : operand"},
			{L"Grammar",		L"Term \"*\" Factor"},
			{L"Grammar",		L"!Term \"*\" Factor:second"},
			{L"Grammar",		L"A B | C D"},
			{L"Grammar",		L"A {B}"},
			{L"Grammar",		L"A [B]"},
			{L"Grammar",		L"[Exp:argument {\",\" Exp:argument}]"},
			{L"Grammar",		L"Term:first \"*\" Factor:second as BinaryExpression with {Operator = \"Mul\"}"},
		};
		Ptr<ParsingTable> table = CreateTable(definition, L"Syngram");
		for (vint i = 0; i < sizeof(inputTexts) / sizeof(*inputTexts); i++)
		{
			Parse(table, inputTexts[i][1], L"Syngram", inputTexts[i][0], i, true, false);
		}
		for (vint i = 0; i < sizeof(inputDefs) / sizeof(*inputDefs); i++)
		{
			WString grammar = ParsingDefinitionToText(inputDefs[i]);
			Parse(table, grammar, L"Syngram_Bootstrap", L"ParserDecl", i, false, false);
		}
	});

	{
		Ptr<ParsingGeneralParser> parser = CreateBootstrapStrictParser();
		Ptr<ParsingDefinition> inputDefs[] =
		{
			LoadDefinition(L"NameList"),
			LoadDefinition(L"Calculator"),
			LoadDefinition(L"Statement"),
			CreateParserDefinition(),
		};

		TEST_CASE(L"Test bootstrap")
		{
			TEST_ASSERT(parser);

			List<Ptr<ParsingError>> errors;
			for (vint i = 0; i < sizeof(inputDefs) / sizeof(*inputDefs); i++)
			{
				WString grammar = ParsingDefinitionToText(inputDefs[i]);
				Ptr<ParsingTreeNode> node = parser->Parse(grammar, L"ParserDecl", errors);
				TEST_ASSERT(node);
				Ptr<ParsingDefinition> def = DeserializeDefinition(node);
				TEST_ASSERT(def);
				WString grammar2 = ParsingDefinitionToText(def);
				TEST_ASSERT(grammar == grammar2);
			}
		});
	}

	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"Calculator");
		TEST_CASE(L"Test character position")
		{
			Ptr<ParsingGeneralParser> parser;
			{
				List<Ptr<ParsingError>> errors;
				Ptr<ParsingTable> table = GenerateTable(definition, false, errors);
				TEST_ASSERT(table);
				parser = CreateStrictParser(table);
			}

			WString input = L"11+22*\r\n33+44";
			List<Ptr<ParsingError>> errors;
			Ptr<ParsingTreeNode> node = parser->Parse(input, L"Exp", errors);
			TEST_ASSERT(node);
			node->InitializeQueryCache();

			{
				ParsingTextPos pos(3);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeToken* token = dynamic_cast<ParsingTreeToken*>(foundNode);
				TEST_ASSERT(token);
				TEST_ASSERT(token->GetValue() == L"22");
			}
			{
				ParsingTextPos pos(4);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeToken* token = dynamic_cast<ParsingTreeToken*>(foundNode);
				TEST_ASSERT(token);
				TEST_ASSERT(token->GetValue() == L"22");
			}
			{
				ParsingTextPos pos(5);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeObject* obj = dynamic_cast<ParsingTreeObject*>(foundNode);
				TEST_ASSERT(obj);
				TEST_ASSERT(obj->GetMember(L"binaryOperator").Cast<ParsingTreeToken>()->GetValue() == L"Mul");
			}
			{
				ParsingTextPos pos(1, 1);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeToken* token = dynamic_cast<ParsingTreeToken*>(foundNode);
				TEST_ASSERT(token);
				TEST_ASSERT(token->GetValue() == L"33");
			}
			{
				ParsingTextPos pos(1, 2);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeObject* obj = dynamic_cast<ParsingTreeObject*>(foundNode);
				TEST_ASSERT(obj);
				TEST_ASSERT(obj->GetMember(L"binaryOperator").Cast<ParsingTreeToken>()->GetValue() == L"Add");
			}
			{
				ParsingTextPos pos(1, 3);
				ParsingTreeNode* foundNode = node->FindDeepestNode(pos);
				TEST_ASSERT(foundNode);
				ParsingTreeToken* token = dynamic_cast<ParsingTreeToken*>(foundNode);
				TEST_ASSERT(token);
				TEST_ASSERT(token->GetValue() == L"44");
			}
		});
	}

	TEST_CATEGORY(L"Auto recovering: Calculator")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"Calculator");
		List<WString> inputs;
		inputs.Add(L"");
		inputs.Add(L"+");
		inputs.Add(L"1+");
		inputs.Add(L"+1");
		inputs.Add(L"(1");
		inputs.Add(L"1)");
		inputs.Add(L"1 2+3)");
		inputs.Add(L"(1 2+3");
		inputs.Add(L"()");
		inputs.Add(L"exec");
		inputs.Add(L"exec (");
		inputs.Add(L"exec )");
		inputs.Add(L"exec exec");
		ParseWithAutoRecover(definition, L"Calculator", L"Exec", inputs, false);
	});

	TEST_CATEGORY(L"Auto recovering: AmbiguousExpression")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"AmbiguousExpression");
		List<WString> inputs;
		inputs.Add(L"");
		inputs.Add(L"a<");
		inputs.Add(L"a>");
		inputs.Add(L"a.");
		inputs.Add(L"<a");
		inputs.Add(L">a");
		inputs.Add(L".a");
		inputs.Add(L"x.a<");
		inputs.Add(L"x.a>");
		inputs.Add(L"x.a.");
		inputs.Add(L"x.<a");
		inputs.Add(L"x.>a");
		inputs.Add(L"x..a");
		inputs.Add(L"x<,a<");
		inputs.Add(L"x<,a>");
		inputs.Add(L"x<,a.");
		inputs.Add(L"x<,<a");
		inputs.Add(L"x<,>a");
		inputs.Add(L"x<,.a");
		ParseWithAutoRecover(definition, L"AmbiguousExpression", L"Exp", inputs, true);
	});
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
