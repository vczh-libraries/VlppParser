#include "ParserGen.h"

void WriteParserFunctions(ParsingSymbolManager* manager, const Dictionary<WString, Pair<WString, WString>>& parsers, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	FOREACH(WString, name, parsers.Keys())
	{
		ParsingSymbol* rule=manager->GetGlobal()->GetSubSymbolByName(parsers[name].key);
		if(rule)
		{
			WString document = parsers[name].value;
			writer.WriteLine(L"");
			if (document != L"")
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(document) + L"</summary>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <returns>Returns the parsing result as a weak-typed AST node. Returns null if there is any unrecoverable error during parsing.</returns>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"input\">The input for parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"table\">The return value from <see cref=\"" + codeClassPrefix + L"LoadTable\"/>.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"errors\">All errors during parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"codeIndex\">(Optional): This argument will be copied to <see cref=\"vl::parsing::ParsingTextRange::codeIndex\"/> in every AST nodes. The default value is -1.</param>");
			}
			writer.WriteString(prefix);
			writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeNode> ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex = -1);");

			if (document != L"")
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(document) + L"</summary>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <returns>Returns the parsing result as a weak-typed AST node. Returns null if there is any unrecoverable error during parsing.</returns>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"input\">The input for parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"table\">The return value from <see cref=\"" + codeClassPrefix + L"LoadTable\"/>.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"codeIndex\">(Optional): This argument will be copied to <see cref=\"vl::parsing::ParsingTextRange::codeIndex\"/> in every AST nodes. The default value is -1.</param>");
			}
			writer.WriteString(prefix);
			writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeNode> ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex = -1);");

			ParsingSymbol* type=rule->GetDescriptorSymbol();
			if (document != L"")
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(document) + L"</summary>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <returns>Returns the parsing result as a strong-typed AST node. Returns null if there is any unrecoverable error during parsing.</returns>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"input\">The input for parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"table\">The return value from <see cref=\"" + codeClassPrefix + L"LoadTable\"/>.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"errors\">All errors during parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"codeIndex\">(Optional): This argument will be copied to <see cref=\"vl::parsing::ParsingTextRange::codeIndex\"/> in every AST nodes. The default value is -1.</param>");
			}
			writer.WriteString(prefix);
			writer.WriteString(L"extern ");
			PrintTypeForValue(type, codeClassPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex = -1);");

			if (document != L"")
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(document) + L"</summary>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <returns>Returns the parsing result as a strong-typed AST node. Returns null if there is any unrecoverable error during parsing.</returns>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"input\">The input for parsing.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"table\">The return value from <see cref=\"" + codeClassPrefix + L"LoadTable\"/>.</param>");
				writer.WriteString(prefix);
				writer.WriteLine(L"/// <param name=\"codeIndex\">(Optional): This argument will be copied to <see cref=\"vl::parsing::ParsingTextRange::codeIndex\"/> in every AST nodes. The default value is -1.</param>");
			}
			writer.WriteString(prefix);
			writer.WriteString(L"extern ");
			PrintTypeForValue(type, codeClassPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex = -1);");
		}
	}
}