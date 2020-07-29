#include "ParserGen.h"

void WriteTokenDefinition(Ptr<ParsingTable> table, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <summary>Token types. Values of enum items will be used in <see cref=\"vl::regex::RegexToken::token\"/>.</summary>");
	writer.WriteString(prefix);
	writer.WriteString(L"enum class ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"ParserTokenIndex");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	for(vint i=0;i<table->GetTokenCount();i++)
	{
		const ParsingTable::TokenInfo& info=table->GetTokenInfo(i);
		if(info.regexTokenIndex!=-1)
		{
			writer.WriteString(prefix);
			writer.WriteLine(L"\t/// <summary>Token " + info.name + L": " + xml::XmlEscapeValue(info.regex) + L"</summary>");
			writer.WriteString(prefix);
			writer.WriteString(L"\t");
			writer.WriteString(info.name);
			writer.WriteString(L" = ");
			writer.WriteString(itow(info.regexTokenIndex));
			writer.WriteLine(L",");
		}
	}
	for(vint i=0;i<table->GetDiscardTokenCount();i++)
	{
		const ParsingTable::TokenInfo& info=table->GetDiscardTokenInfo(i);
		if(info.regexTokenIndex!=-1)
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\t");
			writer.WriteString(info.name);
			writer.WriteString(L" = ");
			writer.WriteString(itow(info.regexTokenIndex));
			writer.WriteLine(L",");
		}
	}
	writer.WriteString(prefix);
	writer.WriteLine(L"};");
}