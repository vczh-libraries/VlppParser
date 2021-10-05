#include "Common.h"
#include "../../Source/Parsing/Json/ParsingJson.h"

using namespace vl::parsing::json;
using namespace test;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

TEST_FILE
{
	TEST_CATEGORY(L"Generated parser: Json")
	{
		const wchar_t* input[] =
		{
			L"{ }",
			L"[ ]",
			L"[ 1 ]",
			L"[ 1 , 2 ]",
			L"[ true, false, null, 1, \"abc\" ]",
			L"[ \"\\b\\f\\n\\r\\t\\\\\\\"abc\\u0041\\u0039\" ]",
			L"{ \"name\" : \"vczh\", \"scores\" : [100, 90, 80, {\"a\":\"b\"}], \"IDE\":{\"VC++\":\"Microsoft\"} }",
		};
		const wchar_t* output[] =
		{
			L"{}",
			L"[]",
			L"[1]",
			L"[1,2]",
			L"[true,false,null,1,\"abc\"]",
			L"[\"\\b\\f\\n\\r\\t\\\\\\\"abcA9\"]",
			L"{\"name\":\"vczh\",\"scores\":[100,90,80,{\"a\":\"b\"}],\"IDE\":{\"VC++\":\"Microsoft\"}}",
		};
		{
			Ptr<ParsingTable> table = JsonLoadTable();
			TestGeneratedParser(input, output, table, L"Json-Generated", L"JRoot", &JsonParse, &JsonToString);
		}
		{
			Ptr<ParsingDefinition> definition = LoadDefinition(L"Json", JsonGetParserTextBuffer());
			Ptr<ParsingTable> table = CreateTable(definition, L"Json");
			for (vint i = 0; i < sizeof(input) / sizeof(*input); i++)
			{
				Parse(table, input[i], L"Json", L"JRoot", i, true, false);
			}
		}
	});
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
