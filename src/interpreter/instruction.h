#pragma once

#include "entry.h"

namespace ts {
	namespace instruction {
		enum InstructionType {
			INVALID_INSTRUCTION, // an instruction with an invalid type will cause the interpreter to stop
			NOOP,
			PUSH, // push a literal value onto the stack, specifies type
			POP, // pop the top of the stack
			JUMP, // jump to a particular instruction
			JUMP_IF_TRUE, // jump to particular insturction if top element on stack is true, pops the element
			JUMP_IF_FALSE, // jump to particular insturction if top element on stack is false, pops the element
			MATHEMATICS, // do a mathematical operation on two values on the stack, and assign result to place on the stack
			LOCAL_ASSIGN, // assign a value from the stack/instruction to a local variable
			LOCAL_ACCESS, // gets the value of a local variable and puts it on the stack
			NEW_FRAME, // create a new stack frame
			DELETE_FRAME, // delete the latest stack frame, pop all values the frame encompassed
		};

		enum AssignOperations {
			INVALID_ASSIGN,
			EQUALS,
			INCREMENT,
			DECREMENT,
		};

		enum MathematicsOperator {
			INVALID_OPERATOR,
			ADDITION,
			SUBTRACT,
			MULTIPLY,
			DIVISION,
			EQUAL,
			STRING_EQUAL,
			STRING_NOT_EQUAL,
			LESS_THAN_EQUAL,
			GREATER_THAN_EQUAL,
			LESS_THAN,
			GREATER_THAN,
			BITWISE_AND,
			BITWISE_OR,
			BITWISE_XOR,
			APPEND,
			SPC,
			TAB,
		};
	}
	
	// instructions form a linked list
	struct Instruction {
		instruction::InstructionType type;
		Instruction* next; // next instruction in linked list
		long int index; // instruction's index in flat array

		union {
			struct {
				Entry entry;
			} push;

			struct {
				union {
					Instruction* instruction;
					long int index;
				};
			} jump;

			struct {
				union {
					Instruction* instruction;
					long int index;
				};
			} jumpIfTrue;

			struct {
				union {
					Instruction* instruction;
					long int index;
				};
			} jumpIfFalse;

			struct {
				relative_stack_location lvalue;
				relative_stack_location rvalue;

				Entry lvalueEntry;
				Entry rvalueEntry;
				
				// the operator this instruction will perform
				instruction::MathematicsOperator operation;
			} mathematics;

			struct {
				int dimensions;
				string destination;
				bool fromStack;
				Entry entry;
				instruction::AssignOperations operation;
			} localAssign;

			struct {
				int dimensions;
				string source;
			} localAccess;

			struct {
				// the amount of entries we want to save from the top of the current frame,
				// these values will get pushed to the next frame down
				unsigned int save;
			} deleteFrame;
		};

		Instruction() {
			this->type = instruction::INVALID_INSTRUCTION;
			this->next = nullptr;
		}

		~Instruction() {
			
		}
	};

	void copyInstruction(Instruction &source, Instruction &destination);

	struct InstructionReturn {
		Instruction* first; // first instruction in mini linked list
		Instruction* last; // last instruction in mini linked list

		InstructionReturn() {
			this->first = nullptr;
			this->last = nullptr;
		}

		InstructionReturn(Instruction* first, Instruction* last) {
			this->first = first;
			this->last = last;
		}

		void add(Instruction* instruction) {
			if(this->first == nullptr) {
				this->first = instruction;
				this->last = instruction;
			}
			else {
				this->last->next = instruction;
				this->last = instruction;
			}
		}

		void add(InstructionReturn compiled) {
			if(this->first == nullptr) {
				this->first = compiled.first;
				this->last = compiled.last;
			}
			else {
				this->last->next = compiled.first;
				this->last = compiled.last;
			}
		}
	};
}