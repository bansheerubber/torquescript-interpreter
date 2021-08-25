#pragma once

#include <chrono>
#include <cstring>
#include <thread>
#include <queue>
#include <vector>

#include "../args.h"
#include "../util/dynamicArray.h"
#include "entry.h"
#include "function.h"
#include "../io.h"
#include "instruction.h"
#include "instructionContainer.h"
#include "methodTree.h"
#include "../util/minHeap.h"
#include "object.h"
#include "../compiler/package.h"
#include "packagedFunctionList.h"
#include "../include/robin-map/include/tsl/robin_map.h"
#include "schedule.h"
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
	void initSchedule(Interpreter* interpreter, Schedule** schedule);
	
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
			Interpreter(ParsedArguments args, bool isParallel);

			void startInterpretation(Instruction* head);
			void execFile(string filename);
			
			void printStack();
			void warning(const char* format, ...);

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

			void addSchedule(unsigned long long time, string functionName, Entry* arguments, size_t argumentCount, ObjectReference* object = nullptr);

			void tick();
			void setTickRate(long tickRate);

			void setObjectName(string &name, ObjectWrapper* object);
			void deleteObjectName(string &name);

			Entry emptyEntry;

			size_t highestObjectId = 1;

			bool testing = false;
		
		private:
			void interpret(); // interprets the next instruction

			void actuallyExecFile(string filename);

			bool warnings = true;
			bool isParallel = false;
			bool showTime = false;
			
			void push(Entry &entry, instruction::PushType type) __attribute__((always_inline));
			void push(double number, instruction::PushType type) __attribute__((always_inline));
			void push(char* data, instruction::PushType type) __attribute__((always_inline));
			void push(ObjectReference* data, instruction::PushType) __attribute__((always_inline));
			void pop() __attribute__((always_inline));

			size_t ranInstructions = 0;
			unsigned long long startTime;

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
			friend void convertToType(Interpreter* interpreter, Entry &source, entry::EntryType type);
			friend ObjectWrapper* CreateObject(
				class ts::Interpreter* interpreter,
				string nameSpace,
				string inheritedName,
				MethodTree* methodTree,
				MethodTree* typeMethodTree,
				void* data
			);

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
			robin_map<size_t, ObjectWrapper*> objects;
			robin_map<string, ObjectWrapper*> stringToObject;

			// keep track of schedules
			MinHeap<Schedule*, Interpreter> schedules = MinHeap<Schedule*, Interpreter>(this, initSchedule, nullptr);

			// parallel stuff
			thread tickThread;
			queue<string> execFilenames;
			long tickRate = 4;
	};
}