#pragma once

#include "instruction.h"

namespace ts {
	class InstructionContainer {
		public:
			InstructionContainer(ts::Instruction* head);
			void print(); // print all the instructions in this container
		
		private:
			ts::Instruction* array; // pointer to flat array in memory
			unsigned long size;
			friend class Interpreter;
	};
}