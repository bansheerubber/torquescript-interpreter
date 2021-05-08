#pragma once

#include <stack>
#include <vector>

#include "entry.h"
#include "instruction.h"
#include "stack.h"
#include "variableContext.h"

using namespace std;

namespace ts {
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
			
			void interpret(); // interprets the next instruction
			void printInstruction(Instruction* instruction);
			void printStack();
			Instruction* head;
			Instruction* current; // the current instruction
		
		private:
			void push(Entry entry);
			void pop();

			Entry stack[1024];
			stack_location stackPointer = 0; // points to the next valid location on the stack (stackPointer - 1 == top of stack)

			std::stack<StackFrame> frames;

			VariableContext& getTopVariableContext();
			vector<VariableContext> contexts;
	};
}