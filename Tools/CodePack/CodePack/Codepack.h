#include "VlppParser.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::filesystem;
using namespace vl::parsing;
using namespace vl::parsing::xml;
using namespace vl::regex;
using namespace vl::console;
using namespace vl::stream;

extern Regex regexInclude;
extern Regex regexSystemInclude;
extern Regex regexInstruction;

inline WString ReadFile(const FilePath& path)
{
	WString text;
	BomEncoder::Encoding encoding;
	bool containsBom;
	File(path).ReadAllTextWithEncodingTesting(text, encoding, containsBom);
	return text;
}

extern LazyList<FilePath> GetCppFiles(
	const FilePath& folder,
	List<WString>& exceptions
	);

extern LazyList<FilePath> GetHeaderFiles(
	const FilePath& folder,
	List<WString>& exceptions
	);

extern void CategorizeCodeFiles(
	Ptr<XmlDocument> config,
	LazyList<FilePath> files,
	Group<WString, FilePath>& categorizedFiles,
	Dictionary<FilePath, WString>& inputFileToCategories
	);

extern LazyList<FilePath> GetIncludedFiles(
	const FilePath& codeFile,
	Dictionary<FilePath, LazyList<FilePath>>& cachedFileToIncludes,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOns,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOffs
	);

extern void Combine(
	const Dictionary<FilePath, WString>& inputFileToOutputFiles,
	Dictionary<FilePath, LazyList<FilePath>>& cachedFileToIncludes,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOns,
	Group<FilePath, Tuple<WString, FilePath>>& conditionOffs,
	const List<FilePath>& files,
	FilePath outputFilePath,
	FilePath outputIncludeFilePath,
	SortedList<WString>& systemIncludes,
	LazyList<WString> externalIncludes
	);