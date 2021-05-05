#include "interpreter.h"

using namespace ts;

Interpreter::Interpreter() {
	this->contexts.push_back(VariableContext());
}

Interpreter::~Interpreter() {
	Instruction* current = this->head;
	while(current != nullptr) {
		Instruction* temp = current;
		current = current->next;
		delete temp;
	}
}

VariableContext& Interpreter::getTopVariableContext() {
	return this->contexts.back();
}

void Interpreter::interpret() {
	Instruction* instruction = this->current;

	if(instruction == nullptr) { // quit once we run out of instructions
		this->getTopVariableContext().print();
		return;
	}

	this->printInstruction(instruction);
	
	switch(instruction->type) {
		case instruction::INVALID_INSTRUCTION: {
			exit(1);
		}

		case instruction::PUSH: {
			this->stack[this->stackPointer] = Entry(&instruction->push.entry);
			this->stackPointer++;
			break;
		}

		case instruction::POP: {
			if(this->stackPointer == 0) { // protect against empty stack
				exit(1);
			}
			
			this->stack[this->stackPointer - 1] = Entry();
			this->stackPointer--;
			break;
		}

		case instruction::JUMP: {
			this->current = instruction->jump.jumpPoint;
			break;
		}

		case instruction::MATHEMATICS: {
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			this->getTopVariableContext().setVariableEntry(
				instruction->localAssign.destination,
				&instruction->localAssign.entry
			);
			break;
		}
	}

	// if we didn't jump, increment instruction pointer
	if(this->current == instruction) {
		this->current = instruction->next;
	}

	this->interpret();
}

void Interpreter::printInstruction(Instruction* instruction) {
	switch(instruction->type) {
		case instruction::INVALID_INSTRUCTION: {
			printf("INVALID_INSTRUCTION;\n");
			break;
		}

		case instruction::PUSH: {
			printf("PUSH {\n");
			printf("   entry type: %d,\n", instruction->push.entry.type);

			if(instruction->push.entry.type == entry::STRING) {
				printf("   entry data: \"%s\",\n", instruction->push.entry.stringData->c_str());
			}
			else {
				printf("   entry data: %f,\n", *instruction->push.entry.numberData);
			}

			printf("};\n");
			break;
		}

		case instruction::POP: {
			printf("POP;\n");
			break;
		}

		case instruction::JUMP: {
			
			break;
		}

		case instruction::MATHEMATICS: {
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			printf("LOCAL_ASSIGN {\n");

			printf("   destination: %s, \n", instruction->localAssign.destination.c_str());
			printf("   entry type: %d,\n", instruction->localAssign.entry.type);

			if(instruction->localAssign.entry.type == entry::STRING) {
				printf("   entry data: \"%s\",\n", instruction->localAssign.entry.stringData->c_str());
			}
			else {
				printf("   entry data: %f,\n", *instruction->localAssign.entry.numberData);
			}

			printf("};\n");
		}
	}
}