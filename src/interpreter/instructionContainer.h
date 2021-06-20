#pragma once

#include "instruction.h"

namespace ts {
	class InstructionContainer {
		public:
			InstructionContainer();
			InstructionContainer(ts::Instruction* head);
			void print(); // print all the instructions in this container
		
		protected:
			ts::Instruction* array; // pointer to flat array in memory
			unsigned long size;
			friend class Interpreter;
	};
}