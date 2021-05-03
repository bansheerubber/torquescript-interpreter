#pragma once

#include "entry.h"

namespace ts {
	enum InstructionType {
		INVALID_INSTRUCTION, // an instruction with an invalid type will cause the interpreter to stop
		PUSH, // push a literal value onto the stack, specifies type
		POP, // pop the top of the stack
		JUMP, // jump to a particular instruction
		MATHEMATICS, // do a mathematical operation on two values on the stack, and assign result to place on the stack
	};

	enum MathematicsOperator {
		ADDITION,
		SUBTRACT,
		MULTIPLY,
		DIVISION,
	};
	
	// instructions form a linked list
	struct Instruction {
		InstructionType type;
		Instruction* next;

		union {
			struct {
				Entry* entry;
			} push;

			struct {
				Instruction* jumpPoint;
			} jump;

			struct {
				// stack values that are determined upon compilation
				relative_stack_location location1;
				relative_stack_location location2;

				// literal values that are determined upon compilation
				Entry* entry1;
				Entry* entry2;

				// the operator this instruction will perform
				MathematicsOperator operation;
			} mathematics;
		};

		Instruction() {
			this->type = INVALID_INSTRUCTION;
			this->next = nullptr;
		}
	};

	struct InstructionReturn {
		Instruction* first; // first instruction in mini linked list
		Instruction* last; // last instruction in mini linked list
	};
}