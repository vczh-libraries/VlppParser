#include "Codepack.h"

int main(int argc, char* argv[])
{

	Console::SetTitle(L"Vczh CodePack for C++");
	if (argc != 2)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"CodePack.exe <config-xml>");
		Console::SetColor(true, true, true, false);
		return 0;
	}

	// load configuration
	auto workingDir = FilePath(atow(argv[1])).GetFolder();
	Ptr<XmlDocument> config;
	{
		auto text = ReadFile(atow(argv[1]));
		auto table = XmlLoadTable();
		config = XmlParseDocument(text, table);
	}

	// collect code files
	List<FilePath> unprocessedCppFiles;								// all cpp files need to combine
	List<FilePath> unprocessedHeaderFiles;							// all header files need to combine
	Dictionary<FilePath, LazyList<FilePath>> cachedFileToIncludes;	// file -> directly/indirectly included files
	Group<FilePath, Tuple<WString, FilePath>> conditionOns;			// #ifdef -> files to include
	Group<FilePath, Tuple<WString, FilePath>> conditionOffs;		// #ifndef -> files to include
	{
		List<FilePath> folders;
		CopyFrom(
			folders,
			XmlGetElements(XmlGetElement(config->rootElement,L"folders"), L"folder")
				.Select([&](Ptr<XmlElement> e)
				{
					return workingDir / XmlGetAttribute(e, L"path")->value.value;
				})
			);

		List<WString> exceptions;
		CopyFrom(
			exceptions,
			XmlGetElements(XmlGetElement(config->rootElement,L"folders"), L"except")
				.Select([&](Ptr<XmlElement> e)
				{
					return XmlGetAttribute(e, L"pattern")->value.value;
				})
			);

		List<FilePath> headerFiles;

		// enumerate all *.cpp files in specified folders
		CopyFrom(
			unprocessedCppFiles,
			From(folders)
				.SelectMany([&](const FilePath& folder) { return GetCppFiles(folder, exceptions); })
				.Distinct()
			);

		// enumerate all *.h files in specified folders
		CopyFrom(
			headerFiles,
			From(folders)
				.SelectMany([&](const FilePath& folder) { return GetHeaderFiles(folder, exceptions); })
				.Distinct()
			);

		// collect all extra included files from all *.cpp files
		CopyFrom(
			unprocessedHeaderFiles,
			From(headerFiles)
				.Concat(unprocessedCppFiles)
				.SelectMany([&](const FilePath& includedFile)
				{
					return GetIncludedFiles(includedFile, cachedFileToIncludes, conditionOns, conditionOffs);
				})
				.Concat(headerFiles)
				.Distinct()
			);
	}

	// categorize code files
	Group<WString, FilePath> categorizedCppFiles;					// category -> cpp files
	Group<WString, FilePath> categorizedHeaderFiles;				// category -> header files
	Dictionary<FilePath, WString> inputFileToCategories;			// input file -> category
	{
		// categorize all *.cpp and *.h files
		CategorizeCodeFiles(config, unprocessedCppFiles, categorizedCppFiles, inputFileToCategories);
		CategorizeCodeFiles(config, unprocessedHeaderFiles, categorizedHeaderFiles, inputFileToCategories);
	}

	// calculate category dependencies
	PartialOrderingProcessor popCategories;			// POP for category ordering
	Group<vint, WString> componentToCategoryNames;	// component index to category names
	{
		SortedList<FilePath> items;
		Group<FilePath, FilePath> depGroup;

		CopyFrom(items, From(unprocessedCppFiles).Concat(unprocessedHeaderFiles));
		FOREACH(FilePath, filePath, items)
		{
			FOREACH(FilePath, includedFile, GetIncludedFiles(filePath, cachedFileToIncludes, conditionOns, conditionOffs))
			{
				depGroup.Add(filePath, includedFile);
			}
		}

		popCategories.InitWithSubClass(items, depGroup, inputFileToCategories);
		popCategories.Sort();

		for (vint i = 0; i < popCategories.components.Count(); i++)
		{
			auto& component = popCategories.components[i];
			for (vint j = 0; j < component.nodeCount; j++)
			{
				auto& firstNode = popCategories.nodes[component.firstNode[j]];
				auto firstFile = items[firstNode.firstSubClassItem[0]];
				componentToCategoryNames.Add(i, inputFileToCategories[firstFile]);
			}
		}

		bool needExit = false;
		for (vint i = 0; i < componentToCategoryNames.Count(); i++)
		{
			const auto& cycleCategories = componentToCategoryNames.GetByIndex(i);
			if (cycleCategories.Count() > 1)
			{
				Console::SetColor(true, false, false, true);
				Console::WriteLine(
					L"Error: Cycle dependency found in categories: "
					+ From(cycleCategories).Aggregate([](const WString& a, const WString& b) {return a + L", " + b; })
					+ L".");
				Console::SetColor(true, true, true, false);
				needExit = true;
			}
		}
		CHECK_ERROR(!needExit, L"Cycle dependency is not allowed");
	}

	Dictionary<FilePath, WString> inputFileToOutputFiles;			// input file -> output file
	Dictionary<WString, Tuple<WString, bool>> categorizedOutput;	// category -> {output file, generate?}
	{
		// get configuration for all categories
		CopyFrom(
			categorizedOutput,
			XmlGetElements(XmlGetElement(config->rootElement, L"output"), L"codepair")
				.Select([&](Ptr<XmlElement> e)->decltype(categorizedOutput)::ElementType
				{
					return {
						XmlGetAttribute(e, L"category")->value.value,
						{
							XmlGetAttribute(e, L"filename")->value.value,
							XmlGetAttribute(e, L"generate")->value.value == L"true"
						}
					};
				})
		);

		for (vint i = 0; i < inputFileToCategories.Count(); i++)
		{
			auto key = inputFileToCategories.Keys()[i];
			auto value = inputFileToCategories.Values()[i];
			inputFileToOutputFiles.Add(key, categorizedOutput[value].f0);
		}
	}

	// prepare output folder
	auto outputFolder = workingDir / (XmlGetAttribute(XmlGetElement(config->rootElement, L"output"), L"path")->value.value);
	auto outputIncludeOnlyFolder = outputFolder / L"IncludeOnly";
	if (!Folder(outputIncludeOnlyFolder).Exists())
	{
		Folder(outputIncludeOnlyFolder).Create(true);
	}

	// generate code pair header files
	Dictionary<WString, Ptr<SortedList<WString>>> categorizedSystemIncludes;
	for (vint i = 0; i < popCategories.components.Count(); i++)
	{
		auto& component = popCategories.components[i];
		auto categoryName = componentToCategoryNames[i][0];
		auto outputPath = outputFolder / (categorizedOutput[categoryName].f0 + L".h");
		auto outputIncludeOnlyPath = outputIncludeOnlyFolder / (categorizedOutput[categoryName].f0 + L".h");

		auto systemIncludes = MakePtr<SortedList<WString>>();
		categorizedSystemIncludes.Add(categoryName, systemIncludes);

		if (categorizedOutput[categoryName].f1)
		{
			Combine(
				inputFileToOutputFiles,
				cachedFileToIncludes,
				conditionOns,
				conditionOffs,
				categorizedHeaderFiles[categoryName],
				outputPath,
				outputIncludeOnlyPath,
				*systemIncludes.Obj(),
				From(*popCategories.nodes[component.firstNode[0]].ins)
					.Where([&](vint nodeIndex)
					{
						return nodeIndex != component.firstNode[0];
					})
					.Select([&](vint nodeIndex)
					{
						return categorizedOutput[componentToCategoryNames[popCategories.nodes[nodeIndex].component][0]].f0 + L".h";
					})
				);
		}
	}

	// generate code pair cpp files
	for (vint i = 0; i < popCategories.components.Count(); i++)
	{
		auto categoryName = componentToCategoryNames[i][0];
		if (categorizedOutput[categoryName].f1)
		{
			WString outputHeader[] = { categorizedOutput[categoryName].f0 + L".h" };
			auto outputPath = outputFolder / (categorizedOutput[categoryName].f0 + L".cpp");
			auto outputIncludeOnlyPath = outputIncludeOnlyFolder / (categorizedOutput[categoryName].f0 + L".cpp");
			Combine(
				inputFileToOutputFiles,
				cachedFileToIncludes,
				conditionOns,
				conditionOffs,
				categorizedCppFiles[categoryName],
				outputPath,
				outputIncludeOnlyPath,
				*categorizedSystemIncludes[categoryName].Obj(),
				From(outputHeader)
				);
		}
	}

	return 0;
}