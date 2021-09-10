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
#include "../lib/libSymbols.h"
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
	struct FunctionFrame {
		VariableContext* context;
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
	void initSchedule(Interpreter* interpreter, Schedule** schedule);
	
	/*
		the virtual machine manages the stack, variables, and objects. things like functions/classes/etc are handled via the engine
		class, which serves as the global state for the tokenizer/parser/interpreter system
	*/
	class Interpreter {
		friend class Engine;
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
		friend Entry* ts::sl::PARENT(Engine* engine, const char* methodName, size_t argc, Entry* argv, entry::EntryType* argumentTypes);
		friend bool tsTick(tsEngine engine);
		
		public:
			Interpreter();
			~Interpreter();
			Interpreter(class Engine* engine, ParsedArguments args, bool isParallel);

			void startInterpretation(Instruction* head);
			void execFile(string filename);
			
			void printStack();
			void warning(const char* format, ...);

			void addSchedule(unsigned long long time, string functionName, Entry* arguments, size_t argumentCount, ObjectReference* object = nullptr);

			bool tick();
			void setTickRate(long tickRate);

			void setObjectName(string &name, ObjectWrapper* object);
			void deleteObjectName(string &name);

			Entry emptyEntry;

			size_t highestObjectId = 1;

			bool testing = false;

			class Engine* engine = nullptr;

			bool isParallel = false;
		
		private:
			void interpret(); // interprets the next instruction

			void actuallyExecFile(string filename);

			bool warnings = true;
			bool showTime = false;
			
			void push(Entry &entry, instruction::PushType type, bool greedy) __attribute__((always_inline));
			void push(Entry &entry, instruction::PushType type) __attribute__((always_inline));
			void push(double number, instruction::PushType type) __attribute__((always_inline));
			void push(char* data, instruction::PushType type) __attribute__((always_inline));
			void push(ObjectReference* data, instruction::PushType) __attribute__((always_inline));
			void pop() __attribute__((always_inline));

			size_t ranInstructions = 0;
			unsigned long long startTime = 0;

			// stacks
			DynamicArray<Entry, Interpreter> stack = DynamicArray<Entry, Interpreter>(this, 10000, initEntry, nullptr);
			DynamicArray<FunctionFrame, Interpreter> frames = DynamicArray<FunctionFrame, Interpreter>(this, 100, initFunctionFrame, onFunctionFrameRealloc);
			VariableContext* topContext;
			InstructionContainer* topContainer; // the current container we're executing code from, taken from frames
			size_t* instructionPointer; // the current instruction pointer, taken from frames
			size_t stackFramePointer; // the current frame pointer
			Entry returnRegister;
			VariableContext globalContext;

			void pushFunctionFrame(
				InstructionContainer* container,
				PackagedFunctionList* list = nullptr,
				int packagedFunctionListIndex = -1,
				MethodTreeEntry* methodTreeEntry = nullptr,
				int methodTreeEntryIndex = -1,
				size_t argumentCount = 0,
				size_t popCount = 0
			);
			void popFunctionFrame() __attribute__((always_inline));
			void pushTSSLFunctionFrame(MethodTreeEntry* methodTreeEntry, int methodTreeEntryIndex);

			Entry* handleTSSLParent(string &name, size_t argc, Entry* argv, entry::EntryType* argumentTypes);

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