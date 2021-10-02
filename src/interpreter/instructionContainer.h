#pragma once

#include "instruction.h"

namespace ts {
	class InstructionContainer {
		public:
			InstructionContainer();
			InstructionContainer(ts::Instruction* head);
			~InstructionContainer();
			void print(); // print all the instructions in this container
		
		protected:
			ts::Instruction* array = nullptr; // pointer to flat array in memory
			size_t size;
			friend class Interpreter;
	};
}
