#include "interpreter.h"
#include "entry.h"

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

// push an entry onto the stack
void Interpreter::push(Entry &entry) {
	if(this->framePointer != 0) {
		this->frames[this->framePointer - 1].size++; // top frame
	}
	copyEntry(entry, this->stack[this->stackPointer]);
	this->stackPointer++;
}

// push a number onto the stack
void Interpreter::push(double number) {
	if(this->framePointer != 0) {
		this->frames[this->framePointer - 1].size++; // top frame
	}
	this->stack[this->stackPointer].setNumber(number);
	this->stackPointer++;
}

void Interpreter::pop() {
	if(this->stackPointer == 0) { // protect against empty stack
		printError("empty stack\n");
		exit(1);
	}
	
	if(this->framePointer != 0) {
		this->frames[this->framePointer - 1].size--; // top frame
	}
	this->stackPointer--;
}

void Interpreter::startInterpretation(Instruction* head) {
	this->head = head;
	this->current = head;

	this->startTime = chrono::high_resolution_clock::now();

	this->interpret();
}

void Interpreter::interpret() {
	Instruction* instruction = this->current;

	if(instruction == nullptr) { // quit once we run out of instructions
		long int elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->startTime)).count();
		printf("ran %d instructions in %lu us\n", this->ranInstructions, elapsed);
		this->getTopVariableContext().print();
		return;
	}

	// this->printInstruction(instruction);
	
	switch(instruction->type) {
		case instruction::INVALID_INSTRUCTION: {
			printError("invalid instruction\n");
			exit(1);
		}

		case instruction::PUSH: {
			this->push(instruction->push.entry);
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
			Entry* lvalue;
			Entry* rvalue;

			// set lvalue
			if(instruction->mathematics.lvalueEntry.type != entry::INVALID) {
				lvalue = &instruction->mathematics.lvalueEntry;
			}
			else {
				lvalue = &this->stack[instruction->mathematics.lvalue];
			}

			// set rvalue
			if(instruction->mathematics.rvalueEntry.type != entry::INVALID) {
				rvalue = &instruction->mathematics.rvalueEntry;
			}
			else {
				rvalue = &this->stack[instruction->mathematics.rvalue];
			}

			double result;
			bool evaluated = false;
			switch(instruction->mathematics.operation) {
				case instruction::ADDITION: {
					result = lvalue->numberData + rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::SUBTRACT: {
					result = lvalue->numberData - rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::DIVISION: {
					result = lvalue->numberData / rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::MULTIPLY: {
					result = lvalue->numberData * rvalue->numberData;
					evaluated = true;
					break;
				}

				default:
					break;
			}
			
			if(evaluated) {
				this->push(result);
			}
			else {
				printError("could not evaluate math instruction\n");
				exit(1);
			}

			break;
		}

		case instruction::LOCAL_ASSIGN: {
			if(instruction->localAssign.fromStack) {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					this->stack[this->stackPointer - 1]
				);
				this->pop();
			}
			else {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					instruction->localAssign.entry
				);
			}
			break;
		}

		case instruction::NEW_FRAME: {
			this->frames[this->framePointer].start = this->stackPointer;
			this->frames[this->framePointer].size = 0;
			this->framePointer++;
			break;
		}

		// TODO we can probably optimize SavedEntry a little more (not sure if really necessary though)
		case instruction::DELETE_FRAME: {
			StackFrame &frame = this->frames[this->framePointer - 1]; // top frame
			this->framePointer--;

			struct SavedEntry {
				Entry entry;
				SavedEntry* next;
			};

			// save entries to a linked list if needed
			SavedEntry* head = nullptr;
			SavedEntry* current = nullptr;
			int top = frame.start + frame.size;
			for(int i = top - (int)instruction->deleteFrame.save; i < top; i++) {
				if(head == nullptr) {
					current = head = new SavedEntry();
					head->entry = this->stack[i];
				}
				else {
					current->next = new SavedEntry();
					current->next->entry = this->stack[i];
					current = current->next;
				}
			}

			// pop the entire frame
			unsigned int size = frame.size;
			for(unsigned int i = 0; i < size; i++) {
				this->pop();
			}

			// apply saved entries to new top frame
			while(head != nullptr) {
				this->push(head->entry);
				head = head->next;
			}

			break;
		}
	}

	// if we didn't jump, increment instruction pointer
	if(this->current == instruction) {
		this->current = instruction->next;
	}

	this->ranInstructions++;

	this->interpret();
}

void Interpreter::printStack() {
	printf("\nSTACK: %d\n", this->stackPointer);
	for(unsigned int i = 0; i < this->stackPointer; i++) {
		Entry entry = this->stack[i];
		
		printf("ENTRY #%d {\n", i);
		printf("   type: %d,\n", entry.type);

		if(entry.type == entry::STRING) {
			printf("   data: \"%s\",\n", entry.stringData->c_str());
		}
		if(entry.type == entry::NUMBER) {
			printf("   data: %f,\n", entry.numberData);
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
				printf("   entry data: %f,\n", instruction->push.entry.numberData);
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
					printf("   entry data: %f,\n", instruction->localAssign.entry.numberData);
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