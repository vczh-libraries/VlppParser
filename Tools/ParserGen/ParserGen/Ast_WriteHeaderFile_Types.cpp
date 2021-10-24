#include "ParserGen.h"

/***********************************************************************
PrintTypeDefinitionVisitor
***********************************************************************/

class PrintTypeDefinitionVisitor : public Object, public ParsingDefinitionTypeDefinition::IVisitor
{
public:
	ParsingSymbol*					scope;
	ParsingSymbolManager*			manager;
	WString							prefix;
	WString							codeClassPrefix;
	TextWriter&						writer;
	List<ParsingSymbol*>			leafClasses;

	void LogInternal(ParsingDefinitionTypeDefinition* _this, ParsingDefinitionTypeDefinition* _definition, const WString& _prefix)
	{
		ParsingSymbol* oldScope=scope;
		WString oldPrefix=prefix;

		scope=(scope?scope:manager->GetGlobal())->GetSubSymbolByName(_this->name);
		if(!scope) scope=oldScope;
		prefix=_prefix;
		_definition->Accept(this);

		prefix=oldPrefix;
		scope=oldScope;
	}

	PrintTypeDefinitionVisitor(ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _prefix, const WString& _codeClassPrefix, TextWriter& _writer)
		:scope(_scope)
		,manager(_manager)
		,prefix(_prefix)
		,codeClassPrefix(_codeClassPrefix)
		,writer(_writer)
	{
		EnumerateAllLeafClass(manager, (_scope ? _scope : manager->GetGlobal()), leafClasses);
	}

	Ptr<ParsingDefinitionAttribute> GetDocument(ParsingDefinitionBase* node)
	{
		auto attr = From(node->attributes)
			.Where([](Ptr<ParsingDefinitionAttribute> attr)
			{
				return attr->name == L"Document";
			})
			.First({});
		return attr && attr->arguments.Count() > 0 ? attr : nullptr;
	}

	void Visit(ParsingDefinitionClassMemberDefinition* node)override
	{
		writer.WriteString(prefix);
		{
			PrintTypeForValue(node->type.Obj(), scope, manager, codeClassPrefix, writer);
		}
		writer.WriteString(L" ");
		writer.WriteString(node->name);
		writer.WriteLine(L";");
	}

	void Visit(ParsingDefinitionClassDefinition* node)override
	{
		List<ParsingSymbol*> children;
		ParsingSymbol* thisType = (scope ? scope : manager->GetGlobal())->GetSubSymbolByName(node->name);
		SearchChildClasses(thisType, manager->GetGlobal(), manager, children);

		auto classDoc = GetDocument(node);
		if (classDoc)
		{
			writer.WriteString(prefix);
			writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(classDoc->arguments[0]) + L"</summary>");
		}
		writer.WriteString(prefix);
		writer.WriteString(L"class ");
		writer.WriteString(codeClassPrefix);
		writer.WriteString(node->name);
		if(children.Count()>0)
		{
			writer.WriteString(L" abstract");
		}
		writer.WriteString(L" : public ");
		if(node->parentType)
		{
			PrintType(node->parentType.Obj(), scope, manager, codeClassPrefix, writer);
		}
		else
		{
			writer.WriteString(L"vl::parsing::ParsingTreeCustomBase");
		}
		writer.WriteString(L", vl::reflection::Description<");
		writer.WriteString(codeClassPrefix);
		writer.WriteString(node->name);
		writer.WriteLine(L">");

		writer.WriteString(prefix);
		writer.WriteLine(L"{");
		writer.WriteString(prefix);
		writer.WriteLine(L"public:");

		if (children.Count() > 0)
		{
			if (classDoc)
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"\t/// <summary>Visitor interface for <see cref=\"" + codeClassPrefix + node->name + L"\"/>.</summary>");
			}
			writer.WriteString(prefix);
			writer.WriteLine(L"\tclass IVisitor : public vl::reflection::IDescriptable, vl::reflection::Description<IVisitor>");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t{");
			writer.WriteString(prefix);
			writer.WriteLine(L"\tpublic:");

			for (auto child : children)
			{
				if (classDoc)
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"\t\t/// <summary>A callback that is called if the node accepting this visitor is <see cref=\"" + codeClassPrefix + child->GetName() + L"\"/>.</summary>");
					writer.WriteString(prefix);
					writer.WriteLine(L"\t\t/// <param name=\"node\">The strong-typed AST node in its real type.</param>");
				}
				writer.WriteString(prefix);
				writer.WriteString(L"\t\tvirtual void Visit(");
				PrintType(child, codeClassPrefix, writer);
				writer.WriteLine(L"* node)=0;");
			}

			writer.WriteString(prefix);
			writer.WriteLine(L"\t};");
			writer.WriteLine(L"");
			if (classDoc)
			{
				writer.WriteString(prefix);
				writer.WriteLine(L"\t/// <summary>Accept a visitor to reveal its real type of this strong-typed AST node.</summary>");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t/// <param name=\"visitor\">The visitor, one of its <b>Visit</b> method will be called according to the real type of this strong-typed AST node.</param>");
			}
			writer.WriteString(prefix);
			writer.WriteString(L"\tvirtual void Accept(");
			PrintType(thisType, codeClassPrefix, writer);
			writer.WriteLine(L"::IVisitor* visitor)=0;");
			writer.WriteLine(L"");
		}
		
		WriteTypeForwardDefinitions(node->subTypes, prefix+L"\t", thisType, manager, codeClassPrefix, writer);
		WriteTypeDefinitions(node->subTypes, prefix+L"\t", thisType, manager, codeClassPrefix, writer);

		for(int i=0;i<node->members.Count();i++)
		{
			if (classDoc)
			{
				if (auto classFieldDoc = GetDocument(node->members[i].Obj()))
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"\t/// <summary>" + xml::XmlEscapeValue(classFieldDoc->arguments[0]) + L"</summary>");
				}
			}
			LogInternal(node, node->members[i].Obj(), prefix+L"\t");
		}

		if (auto baseType = thisType->GetDescriptorSymbol())
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix);
			writer.WriteString(L"\tvoid Accept(");
			PrintType(baseType, codeClassPrefix, writer);
			writer.WriteLine(L"::IVisitor* visitor)override;");
		}

		if(leafClasses.Contains(thisType))
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix);
			writer.WriteString(L"\tstatic ");
			PrintTypeForValue(thisType, codeClassPrefix, writer);
			writer.WriteLine(L" Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens);");
		}

		writer.WriteString(prefix);
		writer.WriteLine(L"};");
	}

	void Visit(ParsingDefinitionEnumMemberDefinition* node)override
	{
		writer.WriteString(prefix);
		writer.WriteString(node->name);
		writer.WriteLine(L",");
	}

	void Visit(ParsingDefinitionEnumDefinition* node)override
	{
		auto enumDoc = GetDocument(node);
		if (enumDoc)
		{
			writer.WriteString(prefix);
			writer.WriteLine(L"/// <summary>" + xml::XmlEscapeValue(enumDoc->arguments[0]) + L"</summary>");
		}
		writer.WriteString(prefix);
		writer.WriteString(L"enum class ");
		writer.WriteString(codeClassPrefix);
		writer.WriteLine(node->name);
		writer.WriteString(prefix);
		writer.WriteLine(L"{");

		for(int i=0;i<node->members.Count();i++)
		{
			if (enumDoc)
			{
				if (auto enumItemDoc = GetDocument(node->members[i].Obj()))
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"\t/// <summary>" + xml::XmlEscapeValue(enumItemDoc->arguments[0]) + L"</summary>");
				}
			}
			LogInternal(node, node->members[i].Obj(), prefix+L"\t");
		}

		writer.WriteString(prefix);
		writer.WriteLine(L"};");
	}
};

/***********************************************************************
PrintForwardTypeDefinitionVisitor
***********************************************************************/

class PrintForwardTypeDefinitionVisitor : public Object, public ParsingDefinitionTypeDefinition::IVisitor
{
public:
	WString					prefix;
	WString					codeClassPrefix;
	TextWriter&				writer;
	bool					exists;

	PrintForwardTypeDefinitionVisitor(const WString& _prefix, const WString& _codeClassPrefix, TextWriter& _writer)
		:prefix(_prefix)
		,writer(_writer)
		,codeClassPrefix(_codeClassPrefix)
		,exists(false)
	{
	}

	void Visit(ParsingDefinitionClassMemberDefinition* node)override
	{
	}

	void Visit(ParsingDefinitionClassDefinition* node)override
	{
		writer.WriteString(prefix);
		writer.WriteString(L"class ");
		writer.WriteString(codeClassPrefix);
		writer.WriteString(node->name);
		writer.WriteLine(L";");
		exists=true;
	}

	void Visit(ParsingDefinitionEnumMemberDefinition* node)override
	{
	}

	void Visit(ParsingDefinitionEnumDefinition* node)override
	{
	}
};

/***********************************************************************
WriteTypeForwardDefinitions
***********************************************************************/

void WriteTypeForwardDefinitions(List<Ptr<ParsingDefinitionTypeDefinition>>& types, const WString& prefix, ParsingSymbol* scope, ParsingSymbolManager* manager, const WString& codeClassPrefix, TextWriter& writer)
{
	PrintForwardTypeDefinitionVisitor visitor(prefix, codeClassPrefix, writer);
	for (auto type : types)
	{
		type->Accept(&visitor);
	}
	if(visitor.exists)
	{
		writer.WriteLine(L"");
	}
}

/***********************************************************************
WriteTypeDefinitions
***********************************************************************/

void WriteTypeDefinitions(List<Ptr<ParsingDefinitionTypeDefinition>>& types, const WString& prefix, ParsingSymbol* scope, ParsingSymbolManager* manager, const WString& codeClassPrefix, TextWriter& writer)
{
	for (auto type : types)
	{
		PrintTypeDefinitionVisitor visitor(scope, manager, prefix, codeClassPrefix, writer);
		type->Accept(&visitor);
		writer.WriteLine(L"");
	}
}