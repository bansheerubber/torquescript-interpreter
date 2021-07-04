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
	};

	void initFunctionFrame(Interpreter* interpreter, FunctionFrame* frame);
	void onFunctionFrameRealloc(Interpreter* interpreter);
	
	class Interpreter {
		public:
			Interpreter();
			Interpreter(ParsedArguments args);
			~Interpreter();

			void startInterpretation(Instruction* head);
			
			void printStack();
			void warning(const char* format, ...);

			void addFunction(string &name, InstructionReturn output, size_t argumentCount);
			void addFunction(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount);
			void addTSSLFunction(sl::Function* function);

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
			DynamicArray<Entry> stack;
			DynamicArray<FunctionFrame> frames;
			VariableContext* topContext;
			InstructionContainer* topContainer; // the current container we're executing code from, taken from frames
			size_t* instructionPointer; // the current instruction pointer, taken from frames
			size_t stackFramePointer; // the current frame pointer
			Entry returnRegister;

			friend void onFunctionFrameRealloc(Interpreter* interpreter);
			friend string VariableContext::computeVariableString(Instruction &instruction, string &variable);
			friend VariableContext;

			void pushInstructionContainer(InstructionContainer* container, size_t argumentCount = 0, size_t popCount = 0);
			void popInstructionContainer();

			// function datastructures
			robin_map<string, size_t> nameToIndex;
			vector<Function*> functions;

			robin_map<string, size_t> namespaceToIndex;
			vector<NamespaceFunctions*> namespaceFunctions;
	};
}