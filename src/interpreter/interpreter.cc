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

void Interpreter::push(Entry entry) {
	if(this->frames.size() > 0) {
		StackFrame& frame = this->frames.top();
		frame.size++;
	}
	this->stack[this->stackPointer] = entry;
	this->stackPointer++;
}

void Interpreter::pop() {
	if(this->stackPointer == 0) { // protect against empty stack
		exit(1);
	}
	
	if(this->frames.size() > 0) {
		StackFrame& frame = this->frames.top();
		frame.size--;
	}
	this->stack[this->stackPointer - 1] = Entry();
	this->stackPointer--;
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
			this->push(Entry(&instruction->push.entry));
			break;
		}

		case instruction::POP: {
			this->pop();
			break;
		}

		case instruction::JUMP: {
			this->current = instruction->jump.jumpPoint;
			break;
		}

		case instruction::MATHEMATICS: {
			Entry lvalue = this->stack[instruction->mathematics.lvalue];
			Entry rvalue = this->stack[instruction->mathematics.rvalue];

			Entry result;
			switch(instruction->mathematics.operation) {
				case instruction::ADDITION: {
					result.setNumber((*lvalue.numberData) + (*rvalue.numberData));
					break;
				}

				case instruction::SUBTRACT: {
					result.setNumber((*lvalue.numberData) - (*rvalue.numberData));
					break;
				}

				case instruction::DIVISION: {
					result.setNumber((*lvalue.numberData) / (*rvalue.numberData));
					break;
				}

				case instruction::MULTIPLY: {
					result.setNumber((*lvalue.numberData) * (*rvalue.numberData));
					break;
				}

				default:
					break;
			}
			
			if(result.type != entry::INVALID) {
				this->push(result);
			}

			break;
		}

		case instruction::LOCAL_ASSIGN: {
			if(instruction->localAssign.fromStack) {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					&this->stack[this->stackPointer - 1]
				);
				this->pop();
			}
			else {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					&instruction->localAssign.entry
				);
			}
			break;
		}

		case instruction::NEW_FRAME: {
			this->frames.push({
				start: this->stackPointer,
				size: 0,
			});
			break;
		}

		case instruction::DELETE_FRAME: {
			vector<Entry> entries; // entries to save

			StackFrame& frame = this->frames.top();
			int top = frame.start + frame.size - 1; 
			for(int i = top; i > top - (int)instruction->deleteFrame.save; i--) {
				entries.push_back(this->stack[i]);
			}

			unsigned int size = frame.size;
			for(unsigned int i = 0; i < size; i++) {
				this->pop();
			}
			this->frames.pop();

			// apply saved entries to new top frame
			for(Entry entry: entries) {
				this->push(entry);
			}

			break;
		}
	}

	// if we didn't jump, increment instruction pointer
	if(this->current == instruction) {
		this->current = instruction->next;
	}

	this->interpret();
}

void Interpreter::printStack() {
	printf("\nSTACK: %d\n", this->stackPointer);
	for(unsigned int i = 0; i < this->stackPointer; i++) {
		Entry entry = this->stack[i];
		
		printf("ENTRY {\n");
		printf("   type: %d,\n", entry.type);

		if(entry.type == entry::STRING) {
			printf("   data: \"%s\",\n", entry.stringData->c_str());
		}
		if(entry.type == entry::NUMBER) {
			printf("   data: %f,\n", *entry.numberData);
		}

		printf("};\n");
	}
	printf("\n");
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
			printf("MATHEMATICS {\n");
			printf("   operator type: %d,\n", instruction->mathematics.operation);
			printf("   lvalue: %d,\n", instruction->mathematics.lvalue);
			printf("   rvalue: %d,\n", instruction->mathematics.rvalue);
			printf("};\n");
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			printf("LOCAL_ASSIGN {\n");

			printf("   destination: %s, \n", instruction->localAssign.destination.c_str());

			if(instruction->localAssign.fromStack) {
				printf("   from stack: true, \n");
			}
			else {
				printf("   entry type: %d,\n", instruction->localAssign.entry.type);
	
				if(instruction->localAssign.entry.type == entry::STRING) {
					printf("   entry data: \"%s\",\n", instruction->localAssign.entry.stringData->c_str());
				}
				else {
					printf("   entry data: %f,\n", *instruction->localAssign.entry.numberData);
				}
			}

			printf("};\n");
			break;
		}

		case instruction::NEW_FRAME: {
			printf("NEW_FRAME;\n");
			break;
		}

		case instruction::DELETE_FRAME: {
			printf("DELETE_FRAME {\n");
			printf("   save: %d,\n", instruction->deleteFrame.save);
			printf("};\n");
			break;
		}
	}
}