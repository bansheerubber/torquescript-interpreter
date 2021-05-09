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

	// flatten instructions into array so CPU can cache instruction data types (improves performance by 20%)
	int count = 0;
	Instruction* instruction = this->head;
	while(instruction != nullptr) {
		count++;
		instruction = instruction->next;
	}

	this->instructionArray = new Instruction[count];
	this->instructionArraySize = count;

	instruction = this->head;
	count = 0;
	while(instruction != nullptr) {
		copyInstruction(*instruction, this->instructionArray[count]);
		count++;
		instruction = instruction->next;
	}

	this->startTime = chrono::high_resolution_clock::now();

	this->interpret();
}

void Interpreter::interpret() {
	if(this->instructionPointer >= this->instructionArraySize) { // quit once we run out of instructions
		long int elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->startTime)).count();
		printf("ran %d instructions in %lu us\n", this->ranInstructions, elapsed);
		this->getTopVariableContext().print();
		this->printStack();
		return;
	}

	Instruction* instruction = &this->instructionArray[this->instructionPointer];
	this->instructionPointer++;

	// this->printInstruction(instruction);

	// offset for stack accesses
	relative_stack_location offset = 0;
	if(this->framePointer != 0) {
		offset = this->frames[this->framePointer - 1].start;
	}
	
	switch(instruction->type) {
		case instruction::INVALID_INSTRUCTION: {
			printError("invalid instruction\n");
			exit(1);
		}

		case instruction::PUSH: { // push to the stack
			this->push(instruction->push.entry);
			break;
		}

		case instruction::POP: { // pop from the stack
			this->pop();
			break;
		}

		case instruction::JUMP: { // jump to an instruction
			this->current = instruction->jump.jumpPoint;
			break;
		}

		case instruction::MATHEMATICS: { // do the math
			Entry* lvalue;
			Entry* rvalue;

			// set lvalue
			if(instruction->mathematics.lvalueEntry.type != entry::INVALID) {
				lvalue = &instruction->mathematics.lvalueEntry;
			}
			else {
				lvalue = &this->stack[instruction->mathematics.lvalue + offset]; // index from base of frame
			}

			// set rvalue
			if(instruction->mathematics.rvalueEntry.type != entry::INVALID) {
				rvalue = &instruction->mathematics.rvalueEntry;
			}
			else {
				rvalue = &this->stack[instruction->mathematics.rvalue + offset]; // index from base of frame
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
			variable::Array* head = nullptr;
			variable::Array* last = nullptr;
			
			for(int i = instruction->localAssign.dimensions - 1; i >= 0; i--) {
				Entry &value = this->stack[this->stackPointer - 1 - i]; // start from top of stack
				if(last == nullptr) {
					head = last = new variable::Array(&value);
				}
				else {
					last->next = new variable::Array(&value);
					last = last->next;
				}
			}
			
			if(instruction->localAssign.fromStack) {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					head,
					this->stack[this->stackPointer - 1 - instruction->localAssign.dimensions] // start from top of stack
				);
				this->pop();
			}
			else {
				this->getTopVariableContext().setVariableEntry(
					instruction->localAssign.destination,
					head,
					instruction->localAssign.entry
				);
			}

			for(int i = 0; i < instruction->localAssign.dimensions; i++) {
				this->pop(); // pop the dimensions
			}

			break;
		}

		case instruction::LOCAL_ACCESS: { // push local variable to stack
			variable::Array* head = nullptr;
			variable::Array* last = nullptr;
			
			for(int i = instruction->localAccess.dimensions - 1; i >= 0; i--) {
				Entry &value = this->stack[this->stackPointer - 1 - i]; // start from top of stack
				if(last == nullptr) {
					head = last = new variable::Array(&value);
				}
				else {
					last->next = new variable::Array(&value);
					last = last->next;
				}
			}
			
			this->push(
				this->getTopVariableContext().getVariableEntry(
					instruction->localAccess.source,
					head
				)
			);

			for(int i = 0; i < instruction->localAccess.dimensions; i++) {
				this->pop(); // pop the dimensions
			}

			break;
		}

		case instruction::NEW_FRAME: { // create a new frame
			this->frames[this->framePointer].start = this->stackPointer;
			this->frames[this->framePointer].size = 0;
			this->framePointer++;
			break;
		}

		case instruction::DELETE_FRAME: { // delete the last frame
			StackFrame &frame = this->frames[this->framePointer - 1]; // top frame
			int saveTop = frame.start + frame.size;
			int saveStart = saveTop - instruction->deleteFrame.save;

			// pop the entire frame
			unsigned int size = frame.size;
			for(unsigned int i = 0; i < size; i++) { // pop from top of stack
				this->pop();
			}
			this->framePointer--; // pop the frame

			// move the saved entries onto the new stack
			for(int i = saveStart; i < saveTop; i++) {
				this->push(this->stack[i]);
			}

			break;
		}
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

			if(instruction->mathematics.lvalueEntry.type != entry::INVALID) {
				printf("   lvalue type: %d,\n", instruction->mathematics.lvalueEntry.type);
	
				if(instruction->mathematics.lvalueEntry.type == entry::STRING) {
					printf("   lvalue data: \"%s\",\n", instruction->mathematics.lvalueEntry.stringData->c_str());
				}
				else {
					printf("   lvalue data: %f,\n", instruction->mathematics.lvalueEntry.numberData);
				}
			}
			else {
				printf("   lvalue stack: %d,\n", instruction->mathematics.lvalue);
			}

			if(instruction->mathematics.rvalueEntry.type != entry::INVALID) {
				printf("   rvalue type: %d,\n", instruction->mathematics.rvalueEntry.type);
	
				if(instruction->mathematics.rvalueEntry.type == entry::STRING) {
					printf("   rvalue data: \"%s\",\n", instruction->mathematics.rvalueEntry.stringData->c_str());
				}
				else {
					printf("   rvalue data: %f,\n", instruction->mathematics.rvalueEntry.numberData);
				}
			}
			else {
				printf("   rvalue stack: %d,\n", instruction->mathematics.rvalue);
			}
			
			printf("};\n");
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			printf("LOCAL_ASSIGN {\n");

			printf("   destination: %s, \n", instruction->localAssign.destination.c_str());
			printf("   dimensions: %d, \n", instruction->localAssign.dimensions);

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

		case instruction::LOCAL_ACCESS: {
			printf("LOCAL_ACCESS {\n");
			printf("   source: %s, \n", instruction->localAccess.source.c_str());
			printf("   dimensions: %d, \n", instruction->localAccess.dimensions);
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