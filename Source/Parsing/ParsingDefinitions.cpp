/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "ParsingDefinitions.h"

namespace vl
{
	using namespace collections;

	namespace parsing
	{
		namespace definitions
		{

/***********************************************************************
ParsingDefinitionType(Visitor)
***********************************************************************/

			void ParsingDefinitionPrimitiveType::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionTokenType::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionSubType::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionArrayType::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

/***********************************************************************
ParsingDefinitionTypeDefinition(Visitor)
***********************************************************************/

			void ParsingDefinitionClassMemberDefinition::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionClassDefinition::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionEnumMemberDefinition::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionEnumDefinition::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

/***********************************************************************
ParsingDefinitionGrammar(Visitor)
***********************************************************************/

			void ParsingDefinitionPrimitiveGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionTextGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionSequenceGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionAlternativeGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionLoopGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionOptionalGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionCreateGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionAssignGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionUseGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void ParsingDefinitionSetterGrammar::Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}

/***********************************************************************
ParsingDefinitionTypeWriter
***********************************************************************/

			ParsingDefinitionAttributeWriter::ParsingDefinitionAttributeWriter(const WString& name)
			{
				attribute = Ptr(new ParsingDefinitionAttribute);
				attribute->name=name;
			}

			ParsingDefinitionAttributeWriter::ParsingDefinitionAttributeWriter(const ParsingDefinitionAttributeWriter& attributeWriter)
			{
				attribute=attributeWriter.attribute;
			}

			ParsingDefinitionAttributeWriter& ParsingDefinitionAttributeWriter::Argument(const WString& argument)
			{
				attribute->arguments.Add(argument);
				return *this;
			}

			Ptr<ParsingDefinitionAttribute> ParsingDefinitionAttributeWriter::Attribute()const
			{
				return attribute;
			}

			ParsingDefinitionAttributeWriter Attribute(const WString& name)
			{
				return ParsingDefinitionAttributeWriter(name);
			}

/***********************************************************************
ParsingDefinitionTypeWriter
***********************************************************************/

			ParsingDefinitionTypeWriter::ParsingDefinitionTypeWriter(Ptr<ParsingDefinitionType> internalType)
			{
				type=internalType;
			}

			ParsingDefinitionTypeWriter::ParsingDefinitionTypeWriter(const ParsingDefinitionTypeWriter& typeWriter)
			{
				type=typeWriter.type;
			}

			ParsingDefinitionTypeWriter::ParsingDefinitionTypeWriter(const WString& name)
			{
				auto primitiveType = Ptr(new ParsingDefinitionPrimitiveType);
				primitiveType->name=name;
				type=primitiveType;
			}

			ParsingDefinitionTypeWriter ParsingDefinitionTypeWriter::Sub(const WString& subTypeName)const
			{
				auto subType = Ptr(new ParsingDefinitionSubType);
				subType->parentType=type;
				subType->subTypeName=subTypeName;
				return ParsingDefinitionTypeWriter(subType);
			}

			ParsingDefinitionTypeWriter ParsingDefinitionTypeWriter::Array()const
			{
				auto arrayType = Ptr(new ParsingDefinitionArrayType);
				arrayType->elementType=type;
				return ParsingDefinitionTypeWriter(arrayType);
			}

			Ptr<ParsingDefinitionType> ParsingDefinitionTypeWriter::Type()const
			{
				return type;
			}

			ParsingDefinitionTypeWriter Type(const WString& name)
			{
				return ParsingDefinitionTypeWriter(name);
			}

			ParsingDefinitionTypeWriter TokenType()
			{
				auto type = Ptr(new ParsingDefinitionTokenType);
				return ParsingDefinitionTypeWriter(type);
			}

/***********************************************************************
ParsingDefinitionClassDefinitionWriter
***********************************************************************/

			ParsingDefinitionClassDefinitionWriter::ParsingDefinitionClassDefinitionWriter(const WString& name)
			{
				definition = Ptr(new ParsingDefinitionClassDefinition);
				definition->name=name;
				currentDefinition=definition;
			}

			ParsingDefinitionClassDefinitionWriter::ParsingDefinitionClassDefinitionWriter(const WString& name, const ParsingDefinitionTypeWriter& parentType)
			{
				definition = Ptr(new ParsingDefinitionClassDefinition);
				definition->name=name;
				definition->parentType=parentType.Type();
				currentDefinition=definition;
			}

			ParsingDefinitionClassDefinitionWriter& ParsingDefinitionClassDefinitionWriter::AmbiguousType(const ParsingDefinitionTypeWriter& ambiguousType)
			{
				definition->ambiguousType=ambiguousType.Type();
				return *this;
			}

			ParsingDefinitionClassDefinitionWriter& ParsingDefinitionClassDefinitionWriter::Member(const WString& name, const ParsingDefinitionTypeWriter& type, const WString& unescapingFunction)
			{
				auto member = Ptr(new ParsingDefinitionClassMemberDefinition);
				member->name=name;
				member->type=type.Type();
				member->unescapingFunction=unescapingFunction;
				definition->members.Add(member);
				currentDefinition=member;
				return *this;
			}

			ParsingDefinitionClassDefinitionWriter& ParsingDefinitionClassDefinitionWriter::SubType(const ParsingDefinitionTypeDefinitionWriter& type)
			{
				definition->subTypes.Add(type.Definition());
				return *this;
			}

			ParsingDefinitionClassDefinitionWriter& ParsingDefinitionClassDefinitionWriter::Attribute(const ParsingDefinitionAttributeWriter& attribute)
			{
				currentDefinition->attributes.Add(attribute.Attribute());
				return *this;
			}

			Ptr<ParsingDefinitionTypeDefinition> ParsingDefinitionClassDefinitionWriter::Definition()const
			{
				return definition;
			}

			ParsingDefinitionClassDefinitionWriter Class(const WString& name)
			{
				return ParsingDefinitionClassDefinitionWriter(name);
			}

			ParsingDefinitionClassDefinitionWriter Class(const WString& name, const ParsingDefinitionTypeWriter& parentType)
			{
				return ParsingDefinitionClassDefinitionWriter(name, parentType);
			}

/***********************************************************************
ParsingDefinitionEnumDefinitionWriter
***********************************************************************/

			ParsingDefinitionEnumDefinitionWriter::ParsingDefinitionEnumDefinitionWriter(const WString& name)
			{
				definition = Ptr(new ParsingDefinitionEnumDefinition);
				definition->name=name;
				currentDefinition=definition;
			}

			ParsingDefinitionEnumDefinitionWriter& ParsingDefinitionEnumDefinitionWriter::Member(const WString& name)
			{
				auto member = Ptr(new ParsingDefinitionEnumMemberDefinition);
				member->name=name;
				definition->members.Add(member);
				currentDefinition=member;
				return *this;
			}

			ParsingDefinitionEnumDefinitionWriter& ParsingDefinitionEnumDefinitionWriter::Attribute(const ParsingDefinitionAttributeWriter& attribute)
			{
				currentDefinition->attributes.Add(attribute.Attribute());
				return *this;
			}

			Ptr<ParsingDefinitionTypeDefinition> ParsingDefinitionEnumDefinitionWriter::Definition()const
			{
				return definition;
			}

			ParsingDefinitionEnumDefinitionWriter Enum(const WString& name)
			{
				return ParsingDefinitionEnumDefinitionWriter(name);
			}

/***********************************************************************
ParsingDefinitionGrammarWriter
***********************************************************************/

			ParsingDefinitionGrammarWriter::ParsingDefinitionGrammarWriter(Ptr<ParsingDefinitionGrammar> internalGrammar)
			{
				grammar=internalGrammar;
			}

			ParsingDefinitionGrammarWriter::ParsingDefinitionGrammarWriter(const ParsingDefinitionGrammarWriter& grammarWriter)
			{
				grammar=grammarWriter.grammar;
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::operator+(const ParsingDefinitionGrammarWriter& next)const
			{
				auto sequence = Ptr(new ParsingDefinitionSequenceGrammar);
				sequence->first=grammar;
				sequence->second=next.Grammar();
				return ParsingDefinitionGrammarWriter(sequence);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::operator|(const ParsingDefinitionGrammarWriter& next)const
			{
				auto alternative = Ptr(new ParsingDefinitionAlternativeGrammar);
				alternative->first=grammar;
				alternative->second=next.Grammar();
				return ParsingDefinitionGrammarWriter(alternative);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::operator*()const
			{
				auto loop = Ptr(new ParsingDefinitionLoopGrammar);
				loop->grammar=grammar;
				return ParsingDefinitionGrammarWriter(loop);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::As(const ParsingDefinitionTypeWriter& type)const
			{
				auto create = Ptr(new ParsingDefinitionCreateGrammar);
				create->grammar=grammar;
				create->type=type.Type();
				return ParsingDefinitionGrammarWriter(create);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::operator[](const WString& memberName)const
			{
				auto assign = Ptr(new ParsingDefinitionAssignGrammar);
				assign->grammar=grammar;
				assign->memberName=memberName;
				return ParsingDefinitionGrammarWriter(assign);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::operator!()const
			{
				auto use = Ptr(new ParsingDefinitionUseGrammar);
				use->grammar=grammar;
				return ParsingDefinitionGrammarWriter(use);
			}

			ParsingDefinitionGrammarWriter ParsingDefinitionGrammarWriter::Set(const WString& memberName, const WString& value)const
			{
				auto setter = Ptr(new ParsingDefinitionSetterGrammar);
				setter->grammar=grammar;
				setter->memberName=memberName;
				setter->value=value;
				return ParsingDefinitionGrammarWriter(setter);
			}

			Ptr<ParsingDefinitionGrammar> ParsingDefinitionGrammarWriter::Grammar()const
			{
				return grammar;
			}

			ParsingDefinitionGrammarWriter Rule(const WString& name)
			{
				auto grammar = Ptr(new ParsingDefinitionPrimitiveGrammar);
				grammar->name=name;
				return ParsingDefinitionGrammarWriter(grammar);
			}

			ParsingDefinitionGrammarWriter Text(const WString& text)
			{
				auto grammar = Ptr(new ParsingDefinitionTextGrammar);
				grammar->text=text;
				return ParsingDefinitionGrammarWriter(grammar);
			}

			ParsingDefinitionGrammarWriter Opt(const ParsingDefinitionGrammarWriter& writer)
			{
				auto grammar = Ptr(new ParsingDefinitionOptionalGrammar);
				grammar->grammar=writer.Grammar();
				return ParsingDefinitionGrammarWriter(grammar);
			}

/***********************************************************************
ParsingDefinitionTokenDefinitionWriter
***********************************************************************/

			ParsingDefinitionTokenDefinitionWriter::ParsingDefinitionTokenDefinitionWriter(ParsingDefinitionWriter& _owner, Ptr<ParsingDefinitionTokenDefinition> _token)
				:owner(_owner)
				,token(_token)
			{
			}

			ParsingDefinitionTokenDefinitionWriter& ParsingDefinitionTokenDefinitionWriter::Attribute(const ParsingDefinitionAttributeWriter& attribute)
			{
				token->attributes.Add(attribute.Attribute());
				return *this;
			}

			ParsingDefinitionWriter& ParsingDefinitionTokenDefinitionWriter::EndToken()
			{
				return owner;
			}

/***********************************************************************
ParsingDefinitionRuleDefinitionWriter
***********************************************************************/

			ParsingDefinitionRuleDefinitionWriter::ParsingDefinitionRuleDefinitionWriter(ParsingDefinitionWriter& _owner, Ptr<ParsingDefinitionRuleDefinition> _rule)
				:owner(_owner)
				,rule(_rule)
			{
			}

			ParsingDefinitionRuleDefinitionWriter& ParsingDefinitionRuleDefinitionWriter::Imply(const ParsingDefinitionGrammarWriter& grammar)
			{
				rule->grammars.Add(grammar.Grammar());
				return *this;
			}

			ParsingDefinitionRuleDefinitionWriter& ParsingDefinitionRuleDefinitionWriter::Attribute(const ParsingDefinitionAttributeWriter& attribute)
			{
				rule->attributes.Add(attribute.Attribute());
				return *this;
			}

			ParsingDefinitionWriter& ParsingDefinitionRuleDefinitionWriter::EndRule()
			{
				return owner;
			}

/***********************************************************************
ParsingDefinitionWriter
***********************************************************************/

			ParsingDefinitionWriter::ParsingDefinitionWriter()
			{
				definition = Ptr(new ParsingDefinition);
			}

			ParsingDefinitionWriter& ParsingDefinitionWriter::Type(const ParsingDefinitionTypeDefinitionWriter& type)
			{
				definition->types.Add(type.Definition());
				return *this;
			}

			ParsingDefinitionWriter& ParsingDefinitionWriter::Token(const WString& name, const WString& regex)
			{
				return TokenAtt(name, regex).EndToken();
			}

			ParsingDefinitionTokenDefinitionWriter ParsingDefinitionWriter::TokenAtt(const WString& name, const WString& regex)
			{
				auto token = Ptr(new ParsingDefinitionTokenDefinition);
				token->name=name;
				token->regex=regex;
				token->discard=false;
				definition->tokens.Add(token);
				return ParsingDefinitionTokenDefinitionWriter(*this, token);
			}

			ParsingDefinitionWriter& ParsingDefinitionWriter::Discard(const WString& name, const WString& regex)
			{
				auto token = Ptr(new ParsingDefinitionTokenDefinition);
				token->name=name;
				token->regex=regex;
				token->discard=true;
				definition->tokens.Add(token);
				return *this;
			}

			ParsingDefinitionRuleDefinitionWriter ParsingDefinitionWriter::Rule(const WString& name, const ParsingDefinitionTypeWriter& type)
			{
				auto rule = Ptr(new ParsingDefinitionRuleDefinition);
				rule->name=name;
				rule->type=type.Type();
				definition->rules.Add(rule);
				return ParsingDefinitionRuleDefinitionWriter(*this, rule);
			}

			Ptr<ParsingDefinition> ParsingDefinitionWriter::Definition()const
			{
				return definition;
			}
		}
	}
}
