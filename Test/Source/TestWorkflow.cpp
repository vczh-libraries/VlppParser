#include "Common.h"

using namespace test;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

TEST_FILE
{
#ifdef NDEBUG
	TEST_CATEGORY(L"Workflow")
	{
		Ptr<ParsingDefinition> definition = LoadDefinition(L"Workflow");
		Ptr<ParsingTable> table = CreateTable(definition, L"Workflow", true);
	});
#endif
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
