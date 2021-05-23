#pragma once

#include <chrono>
#include <stack>
#include <vector>

#include "entry.h"
#include "instruction.h"
#include "../io.h"
#include "stack.h"
#include "variableContext.h"
#include "instructionContainer.h"
#include "../include/robin-map/include/tsl/robin_map.h"

using namespace std;

namespace ts {
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();

			void startInterpretation(Instruction* head);
			
			void printStack();

			void addFunction(string &name, InstructionReturn output);

			Entry emptyEntry;
		
		private:
			void interpret(); // interprets the next instruction
			
			void push(Entry &entry);
			void push(double number);
			void push(string* data);
			void pop();

			int ranInstructions = 0;
			chrono::high_resolution_clock::time_point startTime;

			Entry stack[1024];
			stack_location stackPointer = 0; // points to the next valid location on the stack (stackPointer - 1 == top of stack)

			void pushVariableContext();
			void popVariableContext();
			VariableContext &getTopVariableContext();
			VariableContext contexts[256];
			VariableContext* topContext;
			int contextPointer = 0;
			Entry& getVariableEntry(class Instruction &instruction, string &variable);
			friend string VariableContext::computeVariableString(Instruction &instruction, string &variable);

			InstructionContainer* containerStack[1024];
			unsigned long containerStackPointer;
			InstructionContainer* topContainer; // the current container we're executing code from, taken from the containerStack

			unsigned long pointerStack[1024];
			unsigned long pointerStackPointer;
			unsigned long* instructionPointer; // the current instruction pointer, taken from the pointerStack
			void pushInstructionContainer(InstructionContainer* container);
			void popInstructionContainer();

			robin_map<string, int> nameToIndex;
			robin_map<int, InstructionContainer*> indexToFunction;
			unsigned long functionCount = 0;
	};
}