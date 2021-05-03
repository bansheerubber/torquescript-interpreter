#include "interpreter.h"

using namespace ts;

void Interpreter::interpret() {
	Instruction* instruction = this->current;

	if(instruction == nullptr) { // quit once we run out of instructions
		return;
	}
	
	switch(instruction->type) {
		case INVALID_INSTRUCTION: {
			exit(1);
		}

		case PUSH: {
			this->stack[this->stackPointer] = Entry(instruction->push.entry);
			this->stackPointer++;
			break;
		}

		case POP: {
			if(this->stackPointer == 0) { // protect against empty stack
				exit(1);
			}
			
			this->stack[this->stackPointer - 1] = Entry();
			this->stackPointer--;
			break;
		}

		case JUMP: {
			this->current = instruction->jump.jumpPoint;
			break;
		}

		case MATHEMATICS: {
			break;
		}
	}

	// if we didn't jump, increment instruction pointer
	if(this->current == instruction) {
		this->current = instruction->next;
	}

	this->interpret();
}