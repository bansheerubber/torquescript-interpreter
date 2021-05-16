#pragma once

#include "instruction.h"

namespace ts {
	class InstructionContainer {
		public:
			InstructionContainer(ts::Instruction* head);
		
		private:
			ts::Instruction* array; // pointer to flat array in memory
			unsigned long size;
			friend class Interpreter;
	};
}