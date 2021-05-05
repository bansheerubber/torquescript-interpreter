#pragma once

#include <vector>

#include "entry.h"
#include "instruction.h"
#include "variableContext.h"

using namespace std;

namespace ts {
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
			
			void interpret(); // interprets the next instruction
			void printInstruction(Instruction* instruction);
			Instruction* head;
			Instruction* current; // the current instruction
		
		private:
			Entry stack[1024];
			stack_location stackPointer = 0; // points to the next valid location on the stack (stackPointer - 1 == top of stack)

			VariableContext& getTopVariableContext();
			vector<VariableContext> contexts;
	};
}