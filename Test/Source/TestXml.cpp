#include "Common.h"
#include "../../Source/Parsing/Xml/ParsingXml.h"

using namespace vl::parsing::xml;

namespace test
{
	WString XmlDocumentToString(Ptr<XmlDocument> doc)
	{
		return XmlToString(doc);
	}
}

using namespace test;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

TEST_FILE
{
	TEST_CATEGORY(L"Generated parser: Xml")
	{
		const wchar_t* input[] =
		{
			L"<name />",
			L"<name att1 = \"value1\" att2 = \"value2\" />",
			L"<?xml version = \"1.0\" encoding = \"utf16\" ?>\r\n<!--this is a comment-->\r\n<name att1 = \"value1\" att2 = \"value2\" />",
			L"<button name = \'&lt;&gt;&amp;&apos;&quot;\'> <![CDATA[ButtonText]]> <![CDATA[!]!]]!]>!>!]]> </button>",
			L"<text> This is a single line of text </text>",
			L"<text> normal <b>bold</b> normal <!--comment--> <i>italic</i> normal </text>",
			L"<text> \"normal\" <b>bold</b> \"normal\' <!--comment--> <i>italic</i> \'normal\" </text>",
		};
		const wchar_t* output[] =
		{
			L"<name/>",
			L"<name att1=\"value1\" att2=\"value2\"/>",
			L"<?xml version=\"1.0\" encoding=\"utf16\"?><!--this is a comment--><name att1=\"value1\" att2=\"value2\"/>",
			L"<button name=\"&lt;&gt;&amp;&apos;&quot;\"><![CDATA[ButtonText]]><![CDATA[!]!]]!]>!>!]]></button>",
			L"<text> This is a single line of text </text>",
			L"<text> normal <b>bold</b> normal <!--comment--><i>italic</i> normal </text>",
			L"<text> &quot;normal&quot; <b>bold</b> &quot;normal&apos; <!--comment--><i>italic</i> &apos;normal&quot; </text>",
		};
		{
			Ptr<ParsingTable> table = XmlLoadTable();
			TestGeneratedParser(input, output, table, L"Xml-Generated", L"XDocument", &XmlParseDocument, &XmlDocumentToString);
		}
		{
			Ptr<ParsingDefinition> definition = LoadDefinition(L"Xml", XmlGetParserTextBuffer());
			Ptr<ParsingTable> table = CreateTable(definition, L"Xml");
			for (vint i = 0; i < sizeof(input) / sizeof(*input); i++)
			{
				Parse(table, input[i], L"Xml", L"XDocument", i, true, false);
			}
		}
	});
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
