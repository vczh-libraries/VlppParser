#include "ParserGen.h"

bool CodegenConfig::ReadConfig(TextReader& reader)
{
	Regex regexInclude(L"^include:(<path>/.+)$");
	Regex regexNamespace(L"^namespace:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexReflection(L"^reflection:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexFilePrefix(L"^filePrefix:(<prefix>/.+)$");
	Regex regexClassPrefix(L"^classPrefix:(<prefix>/.+)$");
	Regex regexClassRoot(L"^classRoot:(<name>/.+)$");
	Regex regexGuard(L"^guard:(<guard>/.+)$");
	Regex regexParser(L"^parser:(<name>/w+)/((<rule>/w+)(,(<document>/.*))?/)$");
	Regex regexFile(L"^file:(<name>/w+)/((<rule>/w+)/)$");
	Regex regexAmbiguity(L"^ambiguity:(<value>enabled|disabled)$");
	Regex regexSerialization(L"^serialization:(<value>enabled|disabled)$");

	vint include_path = regexInclude.CaptureNames().IndexOf(L"path");
	vint namespace_namespace = regexNamespace.CaptureNames().IndexOf(L"namespace");
	vint reflection_namespace = regexReflection.CaptureNames().IndexOf(L"namespace");
	vint file_prefix = regexFilePrefix.CaptureNames().IndexOf(L"prefix");
	vint class_prefix = regexClassPrefix.CaptureNames().IndexOf(L"prefix");
	vint classRoot_name = regexClassRoot.CaptureNames().IndexOf(L"name");
	vint guard_name = regexGuard.CaptureNames().IndexOf(L"guard");
	vint parser_name = regexParser.CaptureNames().IndexOf(L"name");
	vint parser_rule = regexParser.CaptureNames().IndexOf(L"rule");
	vint parser_document = regexParser.CaptureNames().IndexOf(L"document");
	vint file_name = regexFile.CaptureNames().IndexOf(L"name");
	vint file_rule = regexFile.CaptureNames().IndexOf(L"rule");
	vint ambiguity_value = regexAmbiguity.CaptureNames().IndexOf(L"value");
	vint serialization_value = regexSerialization.CaptureNames().IndexOf(L"value");

	CHECK_ERROR(include_path == 0, L"Capture name incorrect.");
	CHECK_ERROR(namespace_namespace == 0, L"Capture name incorrect.");
	CHECK_ERROR(reflection_namespace == 0, L"Capture name incorrect.");
	CHECK_ERROR(file_prefix == 0, L"Capture name incorrect.");
	CHECK_ERROR(class_prefix == 0, L"Capture name incorrect.");
	CHECK_ERROR(classRoot_name == 0, L"Capture name incorrect.");
	CHECK_ERROR(guard_name == 0, L"Capture name incorrect.");
	CHECK_ERROR(parser_name == 0, L"Capture name incorrect.");
	CHECK_ERROR(parser_rule == 1, L"Capture name incorrect.");
	CHECK_ERROR(parser_document == 2, L"Capture name incorrect.");
	CHECK_ERROR(file_name == 0, L"Capture name incorrect.");
	CHECK_ERROR(file_rule == 1, L"Capture name incorrect.");
	CHECK_ERROR(ambiguity_value == 0, L"Capture name incorrect.");
	CHECK_ERROR(serialization_value == 0, L"Capture name incorrect.");

	while (!reader.IsEnd())
	{
		WString line = reader.ReadLine();
		Ptr<RegexMatch> match;
		if (line == L"grammar:")
		{
			break;
		}
		else if ((match = regexInclude.Match(line)) && match->Success())
		{
			includes.Add(match->Groups().Get(include_path).Get(0).Value());
		}
		else if ((match = regexNamespace.Match(line)) && match->Success())
		{
			CopyFrom(codeNamespaces, From(match->Groups().Get(namespace_namespace))
				.Select([=](RegexString s)
				{
					return s.Value();
				}));
		}
		else if ((match = regexReflection.Match(line)) && match->Success())
		{
			CopyFrom(reflectionNamespaces, From(match->Groups().Get(reflection_namespace))
				.Select([=](RegexString s)
				{
					return s.Value();
				}));
		}
		else if ((match = regexFilePrefix.Match(line)) && match->Success())
		{
			filePrefix = match->Groups().Get(file_prefix).Get(0).Value();
		}
		else if ((match = regexClassPrefix.Match(line)) && match->Success())
		{
			classPrefix = match->Groups().Get(class_prefix).Get(0).Value();
		}
		else if ((match = regexClassRoot.Match(line)) && match->Success())
		{
			classRoot = match->Groups().Get(classRoot_name).Get(0).Value();
		}
		else if ((match = regexGuard.Match(line)) && match->Success())
		{
			guard = match->Groups().Get(guard_name).Get(0).Value();
		}
		else if ((match = regexParser.Match(line)) && match->Success())
		{
			WString name = match->Groups().Get(parser_name).Get(0).Value();
			WString rule = match->Groups().Get(parser_rule).Get(0).Value();
			WString document;
			if (match->Groups().Contains(parser_document))
			{
				document = match->Groups().Get(parser_document).Get(0).Value();
			}
			if (!parsers.Keys().Contains(name))
			{
				parsers.Add(name, { rule,document });
			}
		}
		else if ((match = regexFile.Match(line)) && match->Success())
		{
			WString name = match->Groups().Get(file_name).Get(0).Value();
			WString rule = match->Groups().Get(file_rule).Get(0).Value();
			if (!files.Keys().Contains(name))
			{
				files.Add(name, rule);
			}
		}
		else if ((match = regexAmbiguity.Match(line)) && match->Success())
		{
			WString value = match->Groups().Get(ambiguity_value).Get(0).Value();
			ambiguity = value;
		}
		else if ((match = regexSerialization.Match(line)) && match->Success())
		{
			WString value = match->Groups().Get(serialization_value).Get(0).Value();
			serialization = value;
		}
		else
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"error> Unknown property \"" + line + L".");
			Console::SetColor(false, true, false, true);
			return false;
		}
	}

	if (includes.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"include\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (codeNamespaces.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"namespace\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (reflectionNamespaces.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"reflection\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (filePrefix == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"filePrefix\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (classPrefix == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"classPrefix\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (classRoot == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"classRoot\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (guard == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"guard\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (parsers.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"parser\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (!files.Keys().Contains(L"Ast"))
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"file:Ast()\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (!files.Keys().Contains(L"Parser"))
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"file:Parser()\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (ambiguity == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"ambiguity\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (serialization == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"serialization\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	return true;
}