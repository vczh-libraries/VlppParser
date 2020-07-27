/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_PARSING_PARSINGTABLE
#define VCZH_PARSING_PARSINGTABLE

#include "ParsingTree.h"

namespace vl
{
	namespace parsing
	{
		namespace tabling
		{

/***********************************************************************
Parsing Table
***********************************************************************/

			/// <summary>
			/// <p>
			/// The parsing table. When you complete a grammar file, use ParserGen.exe to generate the C++ code for you to create a parsing table.
			/// </p>
			/// <p>
			/// Here is a brief description of the grammar file format:
			/// </p>
			/// <p>
			/// The grammar file consists of four parts: configuration, types, tokens and grammar in order like this:
			/// <program><code><![CDATA[
			/// CONFIGURATION
			/// grammar:
			/// TYPES
			/// TOKENS
			/// GRAMMAR
			/// ]]></code></program>
			/// </p>
			/// <p>
			/// <b>Configuration</b>
			/// <ul>
			/// <li>
			/// <b>include:"releative path to the VlppParser.h"</b>:
			/// (multiple) e.g. "../Import/Vlpp.h"
			/// </li>
			/// <li>
			/// <b>namespace:C++-NAMESPACE</b>:
			/// (single) Namespaces separated by "." to contain the generated code. e.g. vl.parsing.xml
			/// </li>
			/// <li>
			/// <b>reflection:REFLECTION-NAMESPACE</b>:
			/// (single) Namespaces separated by "." to contain the name of reflectable types. In most of the cases this should be the same as namespace. e.g. vl.parsing.xml
			/// </li>
			/// <li>
			/// <b>filePrefix:FILE-PREFIX</b>:
			/// (single) A prefix that will be add before all generated files. e.g. ParsingXml
			/// </li>
			/// <li>
			/// <b>classPrefix:CLASS-PREFIX</b>:
			/// (single) A prefix that will be add before all generated types and function. e.g. Xml
			/// </li>
			/// <li>
			/// <b>classRoot:CLASS-ROOT</b>:
			/// (single) The class that represents the whole text being parsed.
			/// </li>
			/// <li>
			/// <b>guard:C++-HEADER-GUARD</b>:
			/// (single) The C++ header guard pattern macro name. e.g. VCZH_PARSING_XML_PARSINGXML_PARSER
			/// </li>
			/// <li>
			/// <b>parser:NAME(RULE)</b>:
			/// (multiple) Pair a function name to a rule name.
			/// It will generate a function called "XmlParseDocument" to parse the input using the rule named "XDocument",
			/// if you have "classPrefix:Xml" and "parser:ParseDocument(XDocument)".</li>
			/// <li>
			/// <b>file:FEATURE(POSTFIX)</b>:
			/// (multiple) Generate code for a specified feature to "&lt;FILE-PREFIX&gt;&lt;POSTFIX&gt;.h" and "&lt;FILE-PREFIX&gt;&lt;POSTFIX&gt;.cpp".
			/// FEATURE could be Ast, Parser, Copy, Traverse and Empty.
			/// Ast is the definition of all classes in the grammar file.
			/// Parser is the implementation of all parsers.
			/// Others are visitors for Ast with different traversing features.
			/// </li>
			/// <li>
			/// <b>ambiguity:(enabled|disabled)</b>:
			/// (single) Set to "enabled" indicating that the grammar is by design to have ambiguity.
			/// If ambiguity happens during parser, but here is "disabled", then an error will be generated.
			/// </li>
			/// <li>
			/// <b>serialization:(enabled|disabled)</b>:
			/// (single) Set to "enabled" to serialize the parsing table as binary in the generated C++ code,
			/// so that when the "&lt;CLASS-PREFIX&gt;LoadTable" function is called to load the table,
			/// it can deserialize from the binary data directly,
			/// instead of parsing the grammar again.
			/// But the grammar text will always be in the generated C++ code regardless of the value of "serialization",
			/// it can always be retrived using the "&lt;CLASS-PREFIX&gt;GetParserTextBuffer" function.
			/// </li>
			/// </ul>
			/// </p>
			/// <p>
			/// <b>Character escaping in strings</b>
			/// </p>
			/// <p>
			/// There is only character escaping in strings: "", which means the " character.
			/// For example, "a""b""c" means R"TEXT(a"b"c)TEXT" in C++.
			/// </p>
			/// <p>
			/// <b>Types</b>
			/// </p>
			/// <p>
			/// You can write attributes like @AttributeName("argument1", "argument2", ...) in the middle of types.
			/// But attributes don't affect parsing.
			/// All attribute definitions will be stored in the generated parsing table,
			/// and who uses the table defines how attributes work. Multiple attributes are separated by ",".
			/// </p>
			/// <p>
			/// If you only need parsing, usually you don't need to use attributes.
			/// GacUI will use some attributes to drive colorizer and intellisense.
			/// This part is subject to change in the next version, so it will not be described here.
			/// </p>
			/// <p>
			/// <ul>
			///     <li>
			///         <b>Enum</b>:
			///         <program><code><![CDATA[
			///             enum EnumName <attributes>
			///             {
			///                 Item1 <attributes>,
			///                 Item2 <attributes>,
			///                 ... // cannot skip the last ","
			///             }
			///         ]]></code></program>
			///     </li>
			///     <li>
			///         <p>
			///         <b>Class</b>:
			///         <program><code><![CDATA[
			///             class Name [ambiguous(AmbiguousType)] [: ParentType] <attributes>
			///             {
			///                 Type name [(UnescapingFunction)] <attributes> ;
			///             }
			///         ]]></code></program>
			///         </p>
			///         <p>
			///         UnescapingFunction is a callback,
			///         which will be called after the contained type is fully constructed.
			///         The generated C++ code will define forward declarations of all unescaping functions in the cpp file.
			///         You should implement them in other places, or you will get linking errors.
			///         </p>
			///         <p>
			///         If the grammar enables ambiguity, then the parsing result may contain ambiguous results for the same part of the input. For example, in C++:
			///         <program><code><![CDATA[
			///         A*B;
			///         ]]></code></program>
			///         has two meanings without considering the surrounding context: a multiplication expression or a pointer variable definition.
			///         </p>
			///         <p>
			///         If the grammar doesn't enable ambiguity, it will refuce to generate C++ codes because the grammar is wrong.
			///         Note that it doesn't generate errors for every possible cases of ambiguity.
			///         Do not rely on this to check ambiguity in the grammar.
			///         </p>
			///         <p>
			///         If the grammar enables ambiguity, than the syntax tree should be defined like this:
			///         <program><code><![CDATA[
			///             // when ambiguity happens for Statement, AmbiguiusStatement will be used to container all possible cases
			///             class Statement ambiguous(AmbiguousStatement)
			///             {
			///             }
			///
			///             // so that "AmbiguousStatement" should inherit from "Statement"
			///             class AmbiguousStatement : Statement
			///             {
			///                 // it should called "items", and the type should be an array of the base type
			///                 Statement[] items;
			///             }
			///
			///             class ExpressionStatement : Statement
			///             {
			///                 Expression expression;
			///             }
			///
			///             class VariableDefinitionStatement : Statement
			///             {
			///                 Type type;
			///                 token name;
			///             }
			///         ]]></code></program>
			///         For "A*B;" part in the whole input, it becomes an AmbiguousStatement.
			///         The "items" field contains two instances, which are "ExpressionStatement" and "VariableDefinitionStatement".
			///         You could write C++ code to resolve the ambiguity in later passes.
			///         </p>
			///     </li>
			///     <li>
			///         <p><b>Type references</b>:</p>
			///         <p>
			///         Types can be defined globally or inside classes. Generic type is not supported. You can use the following types for a class field:
			///         <ul>
			///             <li><b>token</b>: Store a token, which will becomes [T:vl.parsing.ParsingToken].</li>
			///             <li><b>ClassName</b>: Instance of a specified type, which will becomes Ptr&lt;ClassName&gt;.</li>
			///             <li>
			///                 <p><b>ClassName[]</b>: Array, which will becomes List&lt;Ptr&lt;ClassName&gt;&gt;. Array of tokens are not supported.</p>
			///                 <p>A class name could also be<b>OuterClass.InnerClass</b>, referring to the "InnerClass" defined inside the "OuterClass".</p>
			///             </li>
			///         </ul>
			///         </p>
			///     </li>
			/// </ul>
			/// </p>
			/// <p>
			/// <b>Token definitions</b>
			/// <program><code><![CDATA[
			///     token TokenName = "regular expression" <attributes>;
			///     discardtoken TokenName = "regular expression";
			/// ]></code></program>
			/// "discardtoken" means that,
			/// if such a token is identified,
			/// it will not appear in the lexical analyzing result.
			/// You cannot use tokens marked with "discardtoken" in the grammar.
			/// </p>
			/// <p>
			/// <b>Grammar</b>
			/// <program><code><![CDATA[
			///     rule RuleType RuleName <attributes>
			///         = Grammar1
			///         = Grammar2
			///         ...
			///         ;
			/// ]></code></program>
			/// <p>
			/// It means rule "RuleName" is defined by those grammars,
			/// and matching this rule will create an instance of "RuleType" or one of its sub classes.
			/// </p>
			/// <p>
			/// Here are all supported grammars that:
			/// <ul>
			///     <li><b>RuleName</b>: Defines an input that matches a rule.</li>
			///     <li><b>TokenName</b>: Defines an input that formed by the specified token.</li>
			///     <li><b>"StringConstant"</b>: Defines an input that formed by exactly the string constant. There should be exactly one token who can only be this string constant.</li>
			///     <li><b>Grammar: FieldName</b>: Defines an input that matches Grammar (should be either a rule name or a token name), and the result will be stored in field "FieldName" of the created object.</li>
			///     <li><b>!Grammar</b>: Defines an input that matches Grammar, and the rule will use the created object from this grammar. The input should still match other part of the rule, but result of other parts are ignored.</li>
			///     <li><b>[Grammar]</b>: Defines an input that, if it matches Grammar, it returns the result from that grammar; otherwise, it returns null.</li>
			///     <li><b>{Grammar}</b>: Defines an input that matches 0, 1 or more Grammar.</li>
			///     <li><b>(Grammar)</b>: Defines an input that matches the the grammar. Brackets is only for changing operator associations.</li>
			///     <li><b>Grammar1 Grammar2</b>: Defines an input that should match Grammar1 followed by Grammar2.</li>
			///     <li><b>Grammar1 | Grammar2</b>: Defines an input that match either Grammar1 or Grammar2. When it matches Grammar1, Grammar2 will be ignored.</li>
			///     <li><b>Grammar as Type</b>: Defines an input that matches the Grammar, and the whole branch of the rule creates an instance of type "Type".</li>
			///     <li><b>Grammar with { FieldName = Value }</b>: Defines an input that matches the Grammar, assign "Value" to the field "FieldName" of the created object.</li>
			/// </ul>
			/// A grammar branch must be "GRAMMAR as TYPE with {Field1 = Value1} with {Field2 = Value2} ...".
			/// </p>
			/// <p>
			/// <b>Example</b>
			/// </p>
			/// <p>
			/// Here is an example to parse expression containing +, -, *, /, () and numbers:
			/// <program><code><![CDATA[
			///     include:"Vlpp.h"
			///     namespace:vl.calculator
			///     reflection:vl.calculator
			///     filePrefix:Calc
			///     classPrefix:Calc
			///     classRoot:Expression
			///     guard:VCZH_CALCULATOR_PARSER
			///     parser:ParseExpression(Expr)
			///     file:Ast(_Ast)
			///     file:Parser(_Parser)
			///     ambiguity:disabled
			///     serialization:enabled
			///     grammar:
			///
			///     class Expression
			///     {
			///     }
			///
			///     enum BinaryOperator
			///     {
			///         Add, Sub, Mul, Div,
			///     }
			///
			///     class NumberExpression : Expression
			///     {
			///         token number;
			///     }
			///
			///     class BinaryExpression : Expression
			///     {
			///         BinaryOperator op;
			///         Expression left;
			///         Expression right;
			///     }
			///
			///     token ADD "\+"
			///     token SUB "-"
			///     token MUL "\*"
			///     token DIV "\/"
			///     token NUMBER "\d+(.\d+)?"
			///     token OPEN "("
			///     token CLOSE ")"
			///     discardtoken SPACE = "/s+";
			///     
			///     rule Expression Factor
			///         = NUMBER : number as NumberExpression
			///         = "(" !Expr ")"
			///         ;
			///     rule Expression Term
			///         = !Factor
			///         = Term : left "*" Factor : right as BinaryExpression with {op = "Mul"}
			///         = Term : left "/" Factor : right as BinaryExpression with {op = "Div"}
			///         ;
			///     rule Expression Expr
			///         = !Term
			///         = Expr : left "+" Term : right as BinaryExpression with {op = "Add"}
			///         = Expr : left "-" Term : right as BinaryExpression with {op = "Sub"}
			///         ;
			/// ]]></code></program>
			/// </p>
			/// <p>
			/// After using ParserGen.exe to generate C++ codes, you can do this:
			/// <program><code><![CDATA[
			///     // this table can be used, please cache the result to improve the performance
			///     auto table = CalcLoadTable();
			///     List<Ptr<ParsingError>> errors;
			///     // it should be a Ptr<CalcExpression>, will returns nullptr if the input contains syntax errors
			///     auto expression = CalcParseExpression(L"(1+2) * (3+4)", table, errors);
			/// ]]></code></program>
			/// You don't need to define the "errors" if you don't actually care how the input is wrong.
			/// There will be a overloaded version of CalcParseExpression that doesn't need the third argument.
			/// </p>
			/// <p>
			/// You can also automatically correct wrong input.
			/// Ifthe input is not too wrong to recognize,
			/// you can still get a syntax tree,
			/// but some fields are nullptr,
			/// with errors filled into the "error" variable.
			/// <program><code><![CDATA[
			///     auto table = CalcLoadTable();                   // Load the table.
			///     ParsingState state(L"(1+2) * (3+4)", table);    // Initialize a state with the input and the table.
			///     state.Reset(L"Expr");                           // Set the rule to parse.
			///     auto parser = CreateAutoRecoverParser(table);   // Create an appropriate automatic error recoverable parser.
			///     List<Ptr<ParsingError>> errors;                 // Define an error list.
			///     auto node = parser->Parse(state, errors);       // Parse to get an abstract syntax tree, which is a Ptr<ParsingTreeNode>.
			///     if (node)
			///     {
			///         auto expression = CalcConvertParsingTreeNode(node, state.GetTokens()).Cast<CalcExpression>();
			///     }
			/// ]]></code></program>
			/// </p>
			/// <p>
			/// After you get a strong typed syntax tree, you can use the generated visitor interface to do something, like evaluate the results of the expression:
			/// <program><code><![CDATA[
			///     class Evaluator : public Object, public virtual CalcExpression::IVisitor
			///     {
			///     private:
			///         double result;
			///
			///         double Call(CalcExpression* node)
			///         {
			///             node->Accept(this);
			///             return result;
			///         }
			///
			///     public:
			///         static double Evaluate(CalcExpression* node)
			///         {
			///             return Evaluator().Call(node);
			///         }
			///
			///         void Visit(CalcNumberExpression* node)override
			///         {
			///             return wtof(node->number.value);
			///         }
			///
			///         void Visit(CalcBinaryExpression* node)override
			///         {
			///             auto left = Calc(node->left.Obj());
			///             auto right = Calc(node->right.Obj());
			///             switch (node->op)
			///             {
			///             case CalcBinaryOperator::Add:
			///                 result = left + right;
			///                 break;
			///             case CalcBinaryOperator::Sub:
			///                 result = left 0 right;
			///                 break;
			///             case CalcBinaryOperator::Mul:
			///                 result = left * right;
			///                 break;
			///             case CalcBinaryOperator::Div:
			///                 result = left / right;
			///                 break;
			///             }
			///         }
			///     };
			///
			///     Nullable<double> EvaluateExpression(const WString& input)
			///     {
			///         static auto table = CalcLoadTable();
			///         auto expression = CalcParseExpression(input, table);
			///         Nulllable<double> result;
			///         if (expression)
			///         {
			///             result = Evaluator::Evaulate(expression.Obj());
			///         }
			///         return result;
			///     }
			/// ]]></code></program>
			/// </p>
			/// </summary>
			class ParsingTable : public Object
			{
			public:
				static const vint							TokenBegin=0;
				static const vint							TokenFinish=1;
				static const vint							NormalReduce=2;
				static const vint							LeftRecursiveReduce=3;
				static const vint							UserTokenStart=4;

				class AttributeInfo : public Object
				{
				public:
					WString									name;
					collections::List<WString>				arguments;

					AttributeInfo(const WString& _name = L"")
						:name(_name)
					{
					}

					AttributeInfo* Argument(const WString& argument)
					{
						arguments.Add(argument);
						return this;
					}
				};

				class AttributeInfoList : public Object
				{
				public:
					collections::List<Ptr<AttributeInfo>>	attributes;

					Ptr<AttributeInfo> FindFirst(const WString& name);
				};

				class TreeTypeInfo
				{
				public:
					WString									type;
					vint									attributeIndex;

					TreeTypeInfo()
						:attributeIndex(-1)
					{
					}

					TreeTypeInfo(const WString& _type, vint _attributeIndex)
						:type(_type)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class TreeFieldInfo
				{
				public:
					WString									type;
					WString									field;
					vint									attributeIndex;

					TreeFieldInfo()
						:attributeIndex(-1)
					{
					}

					TreeFieldInfo(const WString& _type, const WString& _field, vint _attributeIndex)
						:type(_type)
						,field(_field)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class TokenInfo
				{
				public:
					WString									name;
					WString									regex;
					vint									regexTokenIndex;
					vint									attributeIndex;

					TokenInfo()
						:regexTokenIndex(-1)
						,attributeIndex(-1)
					{
					}

					TokenInfo(const WString& _name, const WString& _regex, vint _attributeIndex)
						:name(_name)
						,regex(_regex)
						,regexTokenIndex(-1)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class StateInfo
				{
				public:
					WString									ruleName;
					WString									stateName;
					WString									stateExpression;

					WString									ruleAmbiguousType;		// filled in Initialize()

					StateInfo()
					{
					}

					StateInfo(const WString& _ruleName, const WString& _stateName, const WString& _stateExpression)
						:ruleName(_ruleName)
						,stateName(_stateName)
						,stateExpression(_stateExpression)
					{
					}
				};

				class RuleInfo
				{
				public:
					WString									name;
					WString									type;
					WString									ambiguousType;
					vint									rootStartState;
					vint									attributeIndex;

					RuleInfo()
						:rootStartState(-1)
						,attributeIndex(-1)
					{
					}

					RuleInfo(const WString& _name, const WString& _type, const WString& _ambiguousType, vint _rootStartState, vint _attributeIndex)
						:name(_name)
						,type(_type)
						,ambiguousType(_ambiguousType)
						,rootStartState(_rootStartState)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class Instruction
				{
				public:
					enum InstructionType
					{
						Create,
						Assign,
						Item,
						Using,
						Setter,
						Shift,
						Reduce,
						LeftRecursiveReduce,
					};

					InstructionType							instructionType;
					vint									stateParameter;
					WString									nameParameter;
					WString									value;
					WString									creatorRule;

					Instruction()
						:instructionType(Create)
						,stateParameter(0)
					{
					}

					Instruction(InstructionType _instructionType, vint _stateParameter, const WString& _nameParameter, const WString& _value, const WString& _creatorRule)
						:instructionType(_instructionType)
						,stateParameter(_stateParameter)
						,nameParameter(_nameParameter)
						,value(_value)
						,creatorRule(_creatorRule)
					{
					}
				};

				class LookAheadInfo
				{
				public:
					collections::List<vint>					tokens;
					vint									state;

					LookAheadInfo()
						:state(-1)
					{
					}

					enum PrefixResult
					{
						Prefix,
						Equal,
						NotPrefix,
					};

					static PrefixResult						TestPrefix(Ptr<LookAheadInfo> a, Ptr<LookAheadInfo> b);
					static void								WalkInternal(Ptr<ParsingTable> table, Ptr<LookAheadInfo> previous, vint state, collections::SortedList<vint>& walkedStates, collections::List<Ptr<LookAheadInfo>>& newInfos);
					static void								Walk(Ptr<ParsingTable> table, Ptr<LookAheadInfo> previous, vint state, collections::List<Ptr<LookAheadInfo>>& newInfos);
				};

				class TransitionItem
				{
				public:
					vint									token;
					vint									targetState;
					collections::List<Ptr<LookAheadInfo>>	lookAheads;
					collections::List<vint>					stackPattern;
					collections::List<Instruction>			instructions;

					enum OrderResult
					{
						CorrectOrder,
						WrongOrder,
						SameOrder,
						UnknownOrder,
					};

					TransitionItem(){}

					TransitionItem(vint _token, vint _targetState)
						:token(_token)
						,targetState(_targetState)
					{
					}

					static OrderResult						CheckOrder(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2, OrderResult defaultResult = UnknownOrder);
					static vint								Compare(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2, OrderResult defaultResult);
				};

				class TransitionBag
				{
				public:
					collections::List<Ptr<TransitionItem>>	transitionItems;
				};

			protected:
				// metadata
				bool																ambiguity;
				collections::Array<Ptr<AttributeInfoList>>							attributeInfos;
				collections::Array<TreeTypeInfo>									treeTypeInfos;
				collections::Array<TreeFieldInfo>									treeFieldInfos;

				// LALR table
				vint																tokenCount;			// tokenInfos.Count() + discardTokenInfos.Count()
				vint																stateCount;			// stateInfos.Count()
				collections::Array<TokenInfo>										tokenInfos;
				collections::Array<TokenInfo>										discardTokenInfos;
				collections::Array<StateInfo>										stateInfos;
				collections::Array<RuleInfo>										ruleInfos;
				collections::Array<Ptr<TransitionBag>>								transitionBags;

				// generated data
				Ptr<regex::RegexLexer>												lexer;
				collections::Dictionary<WString, vint>								ruleMap;
				collections::Dictionary<WString, vint>								treeTypeInfoMap;
				collections::Dictionary<collections::Pair<WString, WString>, vint>	treeFieldInfoMap;

				template<typename TIO>
				void IO(TIO& io);

			public:
				ParsingTable(vint _attributeInfoCount, vint _treeTypeInfoCount, vint _treeFieldInfoCount, vint _tokenCount, vint _discardTokenCount, vint _stateCount, vint _ruleCount);
				/// <summary>Deserialize the parsing table from a stream. <see cref="Initialize"/> should be before using this table.</summary>
				/// <param name="input">The stream.</param>
				ParsingTable(stream::IStream& input);
				~ParsingTable();

				/// <summary>Serialize the parsing table to a stream.</summary>
				/// <param name="output">The stream.</param>
				void										Serialize(stream::IStream& output);

				bool										GetAmbiguity();
				void										SetAmbiguity(bool value);

				vint										GetAttributeInfoCount();
				Ptr<AttributeInfoList>						GetAttributeInfo(vint index);
				void										SetAttributeInfo(vint index, Ptr<AttributeInfoList> info);

				vint										GetTreeTypeInfoCount();
				const TreeTypeInfo&							GetTreeTypeInfo(vint index);
				const TreeTypeInfo&							GetTreeTypeInfo(const WString& type);
				void										SetTreeTypeInfo(vint index, const TreeTypeInfo& info);

				vint										GetTreeFieldInfoCount();
				const TreeFieldInfo&						GetTreeFieldInfo(vint index);
				const TreeFieldInfo&						GetTreeFieldInfo(const WString& type, const WString& field);
				void										SetTreeFieldInfo(vint index, const TreeFieldInfo& info);

				vint										GetTokenCount();
				const TokenInfo&							GetTokenInfo(vint token);
				void										SetTokenInfo(vint token, const TokenInfo& info);

				vint										GetDiscardTokenCount();
				const TokenInfo&							GetDiscardTokenInfo(vint token);
				void										SetDiscardTokenInfo(vint token, const TokenInfo& info);

				vint										GetStateCount();
				const StateInfo&							GetStateInfo(vint state);
				void										SetStateInfo(vint state, const StateInfo& info);

				vint										GetRuleCount();
				const RuleInfo&								GetRuleInfo(const WString& ruleName);
				const RuleInfo&								GetRuleInfo(vint rule);
				void										SetRuleInfo(vint rule, const RuleInfo& info);

				const regex::RegexLexer&					GetLexer();
				Ptr<TransitionBag>							GetTransitionBag(vint state, vint token);
				void										SetTransitionBag(vint state, vint token, Ptr<TransitionBag> bag);
				/// <summary>Initialize the parsing table. This function should be called after deserializing the table from a string.</summary>
				void										Initialize();
				bool										IsInputToken(vint regexTokenIndex);
				vint										GetTableTokenIndex(vint regexTokenIndex);
				vint										GetTableDiscardTokenIndex(vint regexTokenIndex);
			};

/***********************************************************************
Helper Functions
***********************************************************************/

			extern void										Log(Ptr<ParsingTable> table, stream::TextWriter& writer);
		}
	}
}

#endif