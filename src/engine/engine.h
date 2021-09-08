#pragma once

#include <string>

#ifdef __linux__
#include <termios.h>
#endif

#include "../args.h"
#include "../util/dynamicArray.h"
#include "../interpreter/function.h"
#include "../interpreter/interpreter.h"
#include "../io.h"
#include "../interpreter/methodTree.h"
#include "../compiler/package.h"
#include "../interpreter/packagedFunctionList.h"
#include "../parser/parser.h"
#include "../include/robin-map/include/tsl/robin_map.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

class FunctionDeclaration;
class NewStatement;

extern struct termios originalTerminalAttributes;

void disableRawMode();

namespace ts {
	void initPackagedFunctionList(class Engine* engine, PackagedFunctionList** list);
	void initMethodTree(class Engine* engine, MethodTree** tree);
	
	class Engine {
		friend Interpreter;
		friend FunctionDeclaration;
		friend NewStatement;
		friend void sl::define(Engine* engine);

		public:
			Engine(ParsedArguments args, bool isParallel = false);
			~Engine();

			Tokenizer* tokenizer;
			Parser* parser;
			Interpreter* interpreter;

			void execFile(string fileName, bool forceExecution = false);
			void execPiped(string piped);
			void execShell(string shell, bool forceExecution = false);

			tsPrintFunction(printFunction) = &printf;
			tsPrintFunction(warningFunction) = &printWarning;
			tsPrintFunction(errorFunction) = &printError;

			tsVPrintFunction(vPrintFunction) = &vprintf;
			tsVPrintFunction(vWarningFunction) = &printWarning;
			tsVPrintFunction(vErrorFunction) = &printError;

			void enterShell();

			double getRandom();
			void setRandomSeed(int seed);
			int getRandomSeed();
		
		private:
			ParsedArguments args;

			string shellBuffer = "";

			void shellPrintBuffer();

			queue<string> fileQueue; // queue for parallel interpreter file execution
			queue<string> shellQueue; // queue for parallel interpreter shell execution

			int randomSeed;

			void defineFunction(string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void defineMethod(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void defineTSSLFunction(sl::Function* function);
			void defineTSSLMethodTree(MethodTree* tree);

			MethodTree* createMethodTreeFromNamespace(string nameSpace);
			MethodTree* getNamespace(string nameSpace);

			MethodTree* createMethodTreeFromNamespaces(
				string namespace1,
				string namespace2 = string(),
				string namespace3 = string(),
				string namespace4 = string(),
				string namespace5 = string()
			);

			void addPackageFunction(Package* package, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void addPackageMethod(Package* package, string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);

			// function data structures
			robin_map<string, size_t> nameToFunctionIndex;
			DynamicArray<PackagedFunctionList*, Engine> functions
				= DynamicArray<PackagedFunctionList*, Engine>(this, 1024, initPackagedFunctionList, nullptr);

			robin_map<string, size_t> namespaceToMethodTreeIndex;
			DynamicArray<MethodTree*, Engine> methodTrees = DynamicArray<MethodTree*, Engine>(this, 1024, initMethodTree, nullptr);

			// used to index into a method tree
			robin_map<string, size_t> methodNameToIndex;
			size_t currentMethodNameIndex = 0;
	};
}
