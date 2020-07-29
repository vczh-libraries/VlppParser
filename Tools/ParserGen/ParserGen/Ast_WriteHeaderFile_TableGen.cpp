#include "ParserGen.h"

void WriteMetaDefinition(const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <summary>Get the grammar definition for this parser.</summary>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <returns>The grammar definition for this parser.</returns>");
	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::WString ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"GetParserTextBuffer();");

	writer.WriteString(prefix);
	writer.WriteLine(L"/// <summary>Convert parser result to a strong typed AST node. Usually you don't need to use this function, unless you are doing meta programming like error recovering or implementing intellisense for an editor.</summary>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <returns>Returns the strong typed AST node.</returns>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <param name=\"node\">The parser result.</param>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <param name=\"tokens\">Tokens for parsing. You can get the <see cref=\"vl::regex::RegexLexer\"/> by calling <see cref=\"vl::parsing::tabling::ParsingTable::GetLexer\"/> from <see cref=\"" + codeClassPrefix + L"LoadTable\"/></param>");
	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeCustomBase> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"ConvertParsingTreeNode(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens);");

	writer.WriteString(prefix);
	writer.WriteLine(L"/// <summary>Create the parser table. You should cache the value if possible, for improving performance.</summary>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <returns>The created parser table.</returns>");
	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::Ptr<vl::parsing::tabling::ParsingTable> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"LoadTable();");
}