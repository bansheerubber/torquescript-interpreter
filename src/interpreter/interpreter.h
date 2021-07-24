#pragma once

#include <chrono>
#include <cstring>
#include <vector>

#include "../args.h"
#include "dynamicArray.h"
#include "entry.h"
#include "function.h"
#include "../io.h"
#include "instruction.h"
#include "instructionContainer.h"
#include "methodTree.h"
#include "../compiler/package.h"
#include "packagedFunctionList.h"
#include "../include/robin-map/include/tsl/robin_map.h"
#include "objectReference.h"
#include "variableContext.h"

#define TS_INTERPRETER_PREFIX true

using namespace std;

namespace ts {
	struct NamespaceFunctions {
		robin_map<string, int> nameToIndex;
		robin_map<string, Function*> nameToFunction;
		vector<Function*> functions;

		NamespaceFunctions() {
			this->nameToIndex = robin_map<string, int>();
			this->nameToFunction = robin_map<string, Function*>();
			this->functions = vector<Function*>();
		}
	};

	struct FunctionFrame {
		VariableContext context;
		InstructionContainer* container;
		size_t instructionPointer;
		size_t stackPointer;
		size_t stackPopCount;
		PackagedFunctionList* packagedFunctionList;
		int packagedFunctionListIndex;
		MethodTreeEntry* methodTreeEntry;
		int methodTreeEntryIndex;
		bool isTSSL;
	};

	void initFunctionFrame(Interpreter* interpreter, FunctionFrame* frame);
	void onFunctionFrameRealloc(Interpreter* interpreter);
	void initPackagedFunctionList(Interpreter* interpreter, PackagedFunctionList** list);
	void initMethodTree(Interpreter* interpreter, MethodTree** tree);
	
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
			Interpreter(ParsedArguments args);

			void startInterpretation(Instruction* head);
			
			void printStack();
			void warning(const char* format, ...);

			void defineFunction(string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void defineMethod(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void defineTSSLFunction(sl::Function* function);
			void defineTSSLMethodTree(MethodTree* tree);

			void addPackageFunction(Package* package, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);
			void addPackageMethod(Package* package, string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount);

			Entry emptyEntry;

			size_t highestObjectId = 0;

			bool testing = false;
		
		private:
			void interpret(); // interprets the next instruction

			bool warnings = true;
			
			void push(Entry &entry) __attribute__((always_inline));
			void push(double number) __attribute__((always_inline));
			void push(char* data) __attribute__((always_inline));
			void push(ObjectReference* data) __attribute__((always_inline));
			void pop() __attribute__((always_inline));

			size_t ranInstructions = 0;
			chrono::high_resolution_clock::time_point startTime;

			// stacks
			DynamicArray<Entry, Interpreter> stack = DynamicArray<Entry, Interpreter>(this, 10000, initEntry, nullptr);
			DynamicArray<FunctionFrame, Interpreter> frames = DynamicArray<FunctionFrame, Interpreter>(this, 1024, initFunctionFrame, onFunctionFrameRealloc);
			VariableContext* topContext;
			InstructionContainer* topContainer; // the current container we're executing code from, taken from frames
			size_t* instructionPointer; // the current instruction pointer, taken from frames
			size_t stackFramePointer; // the current frame pointer
			Entry returnRegister;
			VariableContext globalContext;

			friend void onFunctionFrameRealloc(Interpreter* interpreter);
			friend string VariableContext::computeVariableString(Instruction &instruction, string &variable);
			friend VariableContext;
			friend Object;

			void pushFunctionFrame(
				InstructionContainer* container,
				PackagedFunctionList* list = nullptr,
				int packagedFunctionListIndex = -1,
				MethodTreeEntry* methodTreeEntry = nullptr,
				int methodTreeEntryIndex = -1,
				size_t argumentCount = 0,
				size_t popCount = 0
			) __attribute__((always_inline));
			void popFunctionFrame() __attribute__((always_inline));
			void pushTSSLFunctionFrame(MethodTreeEntry* methodTreeEntry, int methodTreeEntryIndex);

			friend Entry* ts::sl::PARENT(Interpreter* interpreter, const char* methodName, size_t argc, Entry* argv, entry::EntryType* argumentTypes);
			Entry* handleTSSLParent(string &name, size_t argc, Entry* argv, entry::EntryType* argumentTypes);

			// function data structures
			robin_map<string, size_t> nameToFunctionIndex;
			DynamicArray<PackagedFunctionList*, Interpreter> functions = DynamicArray<PackagedFunctionList*, Interpreter>(this, 1024, initPackagedFunctionList, nullptr);

			robin_map<string, size_t> namespaceToMethodTreeIndex;
			DynamicArray<MethodTree*, Interpreter> methodTrees = DynamicArray<MethodTree*, Interpreter>(this, 1024, initMethodTree, nullptr);

			// used to index into a method tree
			robin_map<string, size_t> methodNameToIndex;
			size_t currentMethodNameIndex = 0;

			// used to lookup objects
			robin_map<size_t, Object*> objects;
	};
}