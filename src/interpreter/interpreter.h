#pragma once

#include <chrono>
#include <stack>
#include <vector>

#include "entry.h"
#include "instruction.h"
#include "../io.h"
#include "stack.h"
#include "variableContext.h"

using namespace std;

namespace ts {
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();

			void startInterpretation(Instruction* head);
			
			void printInstruction(Instruction* instruction);
			void printStack();
		
		private:
			void interpret(); // interprets the next instruction
			Instruction* head;
			Instruction* current; // the current instruction
			
			void push(Entry &entry);
			void push(double number);
			void pop();

			int ranInstructions = 0;
			chrono::high_resolution_clock::time_point startTime;

			Entry stack[100000];
			stack_location stackPointer = 0; // points to the next valid location on the stack (stackPointer - 1 == top of stack)
			
			StackFrame frames[1024];
			unsigned int framePointer = 0;

			VariableContext& getTopVariableContext();
			vector<VariableContext> contexts;
	};
}