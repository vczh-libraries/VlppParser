#include "Codepack.h"

Regex regexInclude(LR"/(^\s*#include\s*"(<path>[^"]+)"\s*$)/");
Regex regexSystemInclude(LR"/(^\s*#include\s*<(<path>[^"]+)>\s*$)/");
Regex regexInstruction(LR"/(^\s*\/\*\s*CodePack:(<name>\w+)\(((<param>[^,)]+)(,\s*(<param>[^,)]+))*)?\)\s*\*\/\s*$)/");

LazyList<FilePath> GetIncludedFiles(
	const FilePath& codeFile,
	Dictionary<FilePath, LazyList<FilePath>>& scannedFiles,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOns,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOffs
)
{
	{
		vint index = scannedFiles.Keys().IndexOf(codeFile);
		if (index != -1)
		{
			return scannedFiles.Values()[index];
		}
	}
	Console::SetColor(true, true, false, true);
	Console::WriteLine(L"Scanning file: " + codeFile.GetFullPath());
	Console::SetColor(true, true, true, false);

	List<FilePath> includes;
	StringReader reader(ReadFile(codeFile));
	bool skip = false;
	vint lineIndex = 0;
	while (!reader.IsEnd())
	{
		lineIndex++;
		auto line = reader.ReadLine();
		Ptr<RegexMatch> match;
		if ((match = regexInstruction.MatchHead(line)))
		{
			auto name = match->Groups()[L"name"][0].Value();
			const List<RegexString>* params = nullptr;
			{
				vint index = match->Groups().Keys().IndexOf(L"param");
				if (index != -1)
				{
					params = &match->Groups().GetByIndex(index);
				}
			}

			if (name == L"BeginIgnore")
			{
				if (params == nullptr)
				{
					skip = true;
					continue;
				}
			}
			else if (name == L"EndIgnore")
			{
				if (params == nullptr)
				{
					skip = false;
					continue;
				}
			}
			else if (name == L"ConditionOn")
			{
				if (params && params->Count() == 2)
				{
					conditionOns.Add(codeFile, { params->Get(0).Value(),codeFile.GetFolder() / params->Get(1).Value() });
					continue;
				}
			}
			else if (name == L"ConditionOff")
			{
				if (params && params->Count() == 2)
				{
					conditionOffs.Add(codeFile, { params->Get(0).Value(),codeFile.GetFolder() / params->Get(1).Value() });
					continue;
				}
			}
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"Error: Unrecognizable CodePack instruction: \"" + line + L"\" in file: " + codeFile.GetFullPath() + L" (" + itow(lineIndex) + L")");
			Console::SetColor(true, true, true, false);
		}
		else if ((match = regexInclude.MatchHead(line)))
		{
			if (!skip)
			{
				auto path = codeFile.GetFolder() / match->Groups()[L"path"][0].Value();
				if (!includes.Contains(path))
				{
					includes.Add(path);
				}
			}
		}
	}

	auto result = MakePtr<List<FilePath>>();
	CopyFrom(
		*result.Obj(),
		From(includes)
			.Concat(From(includes).SelectMany([&](const FilePath& includedFile)
			{
				return GetIncludedFiles(includedFile, scannedFiles, conditionOns, conditionOffs);
			}))
			.Distinct()
		);

	scannedFiles.Add(codeFile, result);
	return result;
}