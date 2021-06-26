#include "instructionContainer.h"
#include "debug.h"

using namespace ts;

InstructionContainer::InstructionContainer() {
	
}

InstructionContainer::InstructionContainer(Instruction* head) {
	// flatten instructions into array so CPU can cache instruction data types (improves performance by 20%)
	int count = 0;
	Instruction* instruction = head;
	while(instruction != nullptr) {
		Instruction* temp = instruction;
		instruction = instruction->next;
		temp->index = count; // convert next properties to flat array index
		count++;
	}

	this->array = new Instruction[count];
	this->size = count;

	instruction = head;
	count = 0;
	while(instruction != nullptr) {
		copyInstruction(*instruction, this->array[count]);
		
		// convert jump instruction pointers to indices for flat array
		switch(instruction->type) {
			case instruction::JUMP: {
				this->array[count].jump.index = this->array[count].jump.instruction->index;
				break;
			}

			case instruction::JUMP_IF_TRUE: {
				this->array[count].jumpIfTrue.index = this->array[count].jumpIfTrue.instruction->index;
				break;
			}

			case instruction::JUMP_IF_FALSE: {
				this->array[count].jumpIfFalse.index = this->array[count].jumpIfFalse.instruction->index;
				break;
			}

			default: {
				break;
			}
		}

		count++;
		instruction = instruction->next;
	}

	// delete the linked list
	instruction = head;
	while(instruction != nullptr) {
		Instruction* temp = instruction;
		instruction = instruction->next;
		delete temp;
	}

	// this->print();
}

void InstructionContainer::print() {
	printf("-----------------------------------------------------------------\n");
	for(size_t i = 0; i < this->size; i++) {
		PrintInstruction(this->array[i]);
	}
}