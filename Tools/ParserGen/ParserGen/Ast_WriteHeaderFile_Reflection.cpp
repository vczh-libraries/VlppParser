#include "ParserGen.h"

void WriteTypeReflectionDeclaration(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer)
{
	writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
	List<ParsingSymbol*> types, leafClasses;
	EnumerateAllTypes(manager, manager->GetGlobal(), types);
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);

	for (auto type : types)
	{
		writer.WriteString(prefix);
		writer.WriteString(L"DECL_TYPE_INFO(");
		PrintNamespaces(config.codeNamespaces, writer);
		PrintType(type, config.classPrefix, writer);
		writer.WriteLine(L")");
	}

	for (auto type : types)
	{
		if (type->GetType() == ParsingSymbol::ClassType)
		{
			List<ParsingSymbol*> children;
			SearchChildClasses(type, manager->GetGlobal(), manager, children);
			if (children.Count() > 0)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"DECL_TYPE_INFO(");
				PrintNamespaces(config.codeNamespaces, writer);
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor)");
			}
		}
	}
	
	writer.WriteLine(L"");
	writer.WriteLine(L"#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA");
	writer.WriteLine(L"");
	for (auto type : types)
	{
		if (type->GetType() == ParsingSymbol::ClassType)
		{
			List<ParsingSymbol*> children;
			SearchChildClasses(type, manager->GetGlobal(), manager, children);
			if (children.Count() > 0)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(");
				PrintNamespaces(config.codeNamespaces, writer);
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor)");
				List<ParsingSymbol*> children;
				SearchChildClasses(type, manager->GetGlobal(), manager, children);
				for (auto child : children)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"\tvoid Visit(");
					PrintNamespaces(config.codeNamespaces, writer);
					PrintType(child, config.classPrefix, writer);
					writer.WriteLine(L"* node)override");
					writer.WriteString(prefix);
					writer.WriteLine(L"\t{");
					writer.WriteString(prefix);
					writer.WriteLine(L"\t\tINVOKE_INTERFACE_PROXY(Visit, node);");
					writer.WriteString(prefix);
					writer.WriteLine(L"\t}");
					writer.WriteLine(L"");
				}

				writer.WriteString(prefix);
				writer.WriteString(L"END_INTERFACE_PROXY(");
				PrintNamespaces(config.codeNamespaces, writer);
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor)");
				writer.WriteLine(L"");
			}
		}
	}

	writer.WriteLine(L"#endif");
	writer.WriteLine(L"#endif");

	writer.WriteString(prefix);
	writer.WriteLine(L"/// <summary>Load all reflectable AST types, only available when <b>VCZH_DEBUG_NO_REFLECTION</b> is off.</summary>");
	writer.WriteString(prefix);
	writer.WriteLine(L"/// <returns>Returns true if this operation succeeded.</returns>");

	writer.WriteString(prefix);
	writer.WriteString(L"extern bool ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"LoadTypes();");
}