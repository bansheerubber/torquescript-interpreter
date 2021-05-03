#pragma once

#include <vector>

#include "entry.h"
#include "instruction.h"

using namespace std;

namespace ts {
	class Interpreter {
		private:
			Instruction* head;
			Instruction* current; // the current instruction

			Entry stack[1024];
			stack_location stackPointer; // points to the next valid location on the stack (stackPointer - 1 == top of stack)

			void interpret(); // interprets the next instruction
	};
}