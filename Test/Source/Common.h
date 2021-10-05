#include "../../Source/Parsing/ParsingAutomaton.h"
#include "../../Source/Parsing/Parsing.h"

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
	extern Ptr<ParsingTable> CreateTable(Ptr<ParsingDefinition> definition, const WString& name, bool enableAmbiguity = false);
	extern Ptr<ParsingTreeNode> Parse(Ptr<ParsingTable> table, const WString& input, const WString& name, const WString& rule, vint index, bool showInput, bool autoRecover);
	extern Ptr<ParsingDefinition> LoadDefinition(const WString& parserName, const WString& text);
	extern Ptr<ParsingDefinition> LoadDefinition(const WString& parserName);
	extern WString ParsingDefinitionToText(Ptr<ParsingDefinition> definition);
	extern void ParseWithAutoRecover(Ptr<ParsingDefinition> definition, const WString& name, const WString& rule, List<WString>& inputs, bool enableAmbiguity);

	template<typename T, vint Count>
	void TestGeneratedParser(
		const wchar_t* (&input)[Count],
		const wchar_t* (&output)[Count],
		Ptr<ParsingTable> table,
		const WString& name,
		const WString& rule,
		Ptr<T>(*deserializer)(const WString&, Ptr<ParsingTable>, vint),
		WString(*serializer)(Ptr<T>)
	)
	{
		for (vint i = 0; i < Count; i++)
		{
			Parse(table, input[i], name, rule, i, true, false);
			TEST_CASE(L"Print: " + WString(input[i]))
			{
				Ptr<T> node = deserializer(input[i], table, -1);
				WString text = serializer(node);
				TEST_ASSERT(text == output[i]);
			});
		}
	}
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
