.PHONY: all clean pre-build
.DEFAULT_GOAL := all

CPP_COMPILE_OPTIONS=-I ../../../Import -DVCZH_DEBUG_METAONLY_REFLECTION
include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi
	if ! [ -d ../../Output ]; then mkdir ../../Output; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi
	if [ -d ./../../Output ]; then rm -r ../../Output; fi

all:pre-build ./Bin/UnitTest

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/Parsing.o ./Obj/ParsingAnalyzer.o ./Obj/ParsingAutomaton.o ./Obj/ParsingAutomaton_Closure.o ./Obj/ParsingAutomaton_EPDA.o ./Obj/ParsingAutomaton_GenerateTable.o ./Obj/ParsingAutomaton_JPDA.o ./Obj/ParsingAutomaton_MergeStates.o ./Obj/ParsingAutomaton_NPDA.o ./Obj/ParsingDefinitions.o ./Obj/ParsingDefinitions_CreateParserDefinition.o ./Obj/ParsingLogging.o ./Obj/ParsingState.o ./Obj/ParsingTable.o ./Obj/ParsingTree.o ./Obj/ParsingJson.o ./Obj/ParsingJson_Ast.o ./Obj/ParsingJson_Parser.o ./Obj/ParsingXml.o ./Obj/ParsingXml_Ast.o ./Obj/ParsingXml_Parser.o ./Obj/Common.o ./Obj/TestJson.o ./Obj/TestXml.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../../Import/Vlpp.cpp
	$(CPP_COMPILE)

./Obj/Vlpp.Linux.o: ../../../Import/Vlpp.Linux.cpp
	$(CPP_COMPILE)

./Obj/VlppOS.o: ../../../Import/VlppOS.cpp
	$(CPP_COMPILE)

./Obj/VlppOS.Linux.o: ../../../Import/VlppOS.Linux.cpp
	$(CPP_COMPILE)

./Obj/VlppReflection.o: ../../../Import/VlppReflection.cpp
	$(CPP_COMPILE)

./Obj/VlppRegex.o: ../../../Import/VlppRegex.cpp
	$(CPP_COMPILE)

./Obj/Parsing.o: ../../../Source/Parsing/Parsing.cpp
	$(CPP_COMPILE)

./Obj/ParsingAnalyzer.o: ../../../Source/Parsing/ParsingAnalyzer.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton.o: ../../../Source/Parsing/ParsingAutomaton.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_Closure.o: ../../../Source/Parsing/ParsingAutomaton_Closure.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_EPDA.o: ../../../Source/Parsing/ParsingAutomaton_EPDA.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_GenerateTable.o: ../../../Source/Parsing/ParsingAutomaton_GenerateTable.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_JPDA.o: ../../../Source/Parsing/ParsingAutomaton_JPDA.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_MergeStates.o: ../../../Source/Parsing/ParsingAutomaton_MergeStates.cpp
	$(CPP_COMPILE)

./Obj/ParsingAutomaton_NPDA.o: ../../../Source/Parsing/ParsingAutomaton_NPDA.cpp
	$(CPP_COMPILE)

./Obj/ParsingDefinitions.o: ../../../Source/Parsing/ParsingDefinitions.cpp
	$(CPP_COMPILE)

./Obj/ParsingDefinitions_CreateParserDefinition.o: ../../../Source/Parsing/ParsingDefinitions_CreateParserDefinition.cpp
	$(CPP_COMPILE)

./Obj/ParsingLogging.o: ../../../Source/Parsing/ParsingLogging.cpp
	$(CPP_COMPILE)

./Obj/ParsingState.o: ../../../Source/Parsing/ParsingState.cpp
	$(CPP_COMPILE)

./Obj/ParsingTable.o: ../../../Source/Parsing/ParsingTable.cpp
	$(CPP_COMPILE)

./Obj/ParsingTree.o: ../../../Source/Parsing/ParsingTree.cpp
	$(CPP_COMPILE)

./Obj/ParsingJson.o: ../../../Source/Parsing/Json/ParsingJson.cpp
	$(CPP_COMPILE)

./Obj/ParsingJson_Ast.o: ../../../Source/Parsing/Json/ParsingJson_Ast.cpp
	$(CPP_COMPILE)

./Obj/ParsingJson_Parser.o: ../../../Source/Parsing/Json/ParsingJson_Parser.cpp
	$(CPP_COMPILE)

./Obj/ParsingXml.o: ../../../Source/Parsing/Xml/ParsingXml.cpp
	$(CPP_COMPILE)

./Obj/ParsingXml_Ast.o: ../../../Source/Parsing/Xml/ParsingXml_Ast.cpp
	$(CPP_COMPILE)

./Obj/ParsingXml_Parser.o: ../../../Source/Parsing/Xml/ParsingXml_Parser.cpp
	$(CPP_COMPILE)

./Obj/Common.o: ../../Source/Common.cpp
	$(CPP_COMPILE)

./Obj/TestJson.o: ../../Source/TestJson.cpp
	$(CPP_COMPILE)

./Obj/TestXml.o: ../../Source/TestXml.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../Main.cpp
	$(CPP_COMPILE)
