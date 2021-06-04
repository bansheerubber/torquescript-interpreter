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
			ARGUMENT_ASSIGN, // assign a value from the stack to a local variable, account for argument size
			LOCAL_ASSIGN, // assign a value from the stack/instruction to a local variable
			LOCAL_ACCESS, // gets the value of a local variable and puts it on the stack
			CALL_FUNCTION, // call a globally scoped function
			RETURN, // return from a function without returning a value
			POP_ARGUMENTS, // pop x arguments from the stack, x being obtained from the top of the stack
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
			MODULUS,
			EQUAL,
			LESS_THAN_EQUAL,
			GREATER_THAN_EQUAL,
			LESS_THAN,
			GREATER_THAN,
			BITWISE_AND,
			BITWISE_OR,
			BITWISE_XOR,
			STRING_EQUAL,
			STRING_NOT_EQUAL,
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
				bool pop;
			} jumpIfTrue;

			struct {
				union {
					Instruction* instruction;
					long int index;
				};
				bool pop;
			} jumpIfFalse;

			struct {
				Entry lvalueEntry;
				Entry rvalueEntry;
				instruction::MathematicsOperator operation; // the operator this instruction will perform
			} mathematics;

			struct {
				int dimensions;
				string destination;
				size_t hash;
				bool fromStack;
				bool pushResult;
				Entry entry;
				instruction::AssignOperations operation;
			} localAssign;

			struct {
				int dimensions;
				relative_stack_location offset; // subtracted from top of stack
				unsigned int argc; // expected amount of arguments
				string destination;
				size_t hash;
			} argumentAssign;

			struct {
				int dimensions;
				string source;
				size_t hash;
			} localAccess;

			struct {
				string name;
				// cache the index when we lookup the name of the function at runtime
				// (hashing an int during runtime is probably faster than hashing a string)
				unsigned long cachedIndex;
				bool isCached;
				bool isTSSL;
			} callFunction;
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
			if(instruction != nullptr) {
				if(this->first == nullptr) {
					this->first = instruction;
					this->last = instruction;
				}
				else {
					this->last->next = instruction;
					this->last = instruction;
				}
			}
		}

		void add(InstructionReturn compiled) {
			if(compiled.first != nullptr && compiled.last != nullptr) {
				if(this->first == nullptr) {
					this->first = compiled.first;
					this->last = compiled.last;
				}
				else {
					this->last->next = compiled.first;
					this->last = compiled.last;
				}
			}
		}
	};
}