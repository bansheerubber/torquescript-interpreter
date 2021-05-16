#include "interpreter.h"
#include "entry.h"
#include "debug.h"

using namespace ts;

Interpreter::Interpreter() {
	this->pushVariableContext();
}

Interpreter::~Interpreter() {
	
}

void Interpreter::pushInstructionContainer(InstructionContainer* container) {
	// push container
	this->topContainer = container;
	this->containerStack.push(container);

	// push pointer
	this->pointerStack.push(new unsigned long);
	this->instructionPointer = this->pointerStack.top();
	*this->instructionPointer = 0;
}

void Interpreter::popInstructionContainer() {
	// pop container
	this->containerStack.pop();
	this->topContainer = this->containerStack.top();

	// pop pointer
	unsigned long* oldPointer = this->pointerStack.top();
	this->pointerStack.pop();
	delete oldPointer; // free old pointer
	this->instructionPointer = this->pointerStack.top();
}

void Interpreter::pushVariableContext() {
	this->contextPointer++;
	this->getTopVariableContext().interpreter = this;
}

void Interpreter::popVariableContext() {
	this->getTopVariableContext().clear();
	this->contextPointer--;
}

VariableContext& Interpreter::getTopVariableContext() {
	return this->contexts[this->contextPointer - 1];
}

// push an entry onto the stack
void Interpreter::push(Entry &entry) {
	if(this->framePointer != 0) {
		this->topFrame->size++;
	}
	copyEntry(entry, this->stack[this->stackPointer]);
	this->stackPointer++;
}

// push a number onto the stack
void Interpreter::push(double number) {
	if(this->framePointer != 0) {
		this->topFrame->size++;
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
		this->topFrame->size--;
	}
	this->stackPointer--;
}

void Interpreter::startInterpretation(Instruction* head) {
	this->pushInstructionContainer(new InstructionContainer(head)); // create the instructions
	this->startTime = chrono::high_resolution_clock::now();
	this->interpret();
}

void Interpreter::interpret() {
	if(*this->instructionPointer >= this->topContainer->size) { // quit once we run out of instructions
		long int elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->startTime)).count();
		printf("ran %d instructions in %lu us\n", this->ranInstructions, elapsed);
		this->getTopVariableContext().print();
		this->printStack();
		return;
	}

	Instruction &instruction = this->topContainer->array[*this->instructionPointer];
	(*this->instructionPointer)++;

	// PrintInstruction(instruction);
	
	switch(instruction.type) {
		case instruction::INVALID_INSTRUCTION: {
			printError("invalid instruction\n");
			exit(1);
		}
		
		case instruction::NOOP: {
			break;
		}

		case instruction::PUSH: { // push to the stack
			this->push(instruction.push.entry);
			break;
		}

		case instruction::POP: { // pop from the stack
			this->pop();
			break;
		}

		case instruction::JUMP: { // jump to an instruction
			*this->instructionPointer = instruction.jump.index;
			break;
		}

		case instruction::JUMP_IF_TRUE: { // jump to an instruction
			if(this->stack[this->stackPointer - 1].numberData == 1) {
				*this->instructionPointer = instruction.jumpIfTrue.index;
			}
			this->pop();
			break;
		}

		case instruction::JUMP_IF_FALSE: { // jump to an instruction
			if(this->stack[this->stackPointer - 1].numberData == 0) {
				*this->instructionPointer = instruction.jumpIfFalse.index;
			}
			this->pop();
			break;
		}

		case instruction::MATHEMATICS: { // do the math
			// offset for stack accesses
			relative_stack_location offset = 0;
			if(this->framePointer != 0) {
				offset = this->topFrame->start;
			}
			
			Entry* lvalue;
			Entry* rvalue;

			// set lvalue
			if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {
				lvalue = &instruction.mathematics.lvalueEntry;
			}
			else {
				lvalue = &this->stack[instruction.mathematics.lvalue + offset]; // index from base of frame
			}

			// set rvalue
			if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {
				rvalue = &instruction.mathematics.rvalueEntry;
			}
			else {
				rvalue = &this->stack[instruction.mathematics.rvalue + offset]; // index from base of frame
			}

			double result;
			bool evaluated = false;
			switch(instruction.mathematics.operation) {
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

				case instruction::EQUAL: {
					result = lvalue->numberData == rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::LESS_THAN_EQUAL: {
					result = lvalue->numberData <= rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::GREATER_THAN_EQUAL: {
					result = lvalue->numberData >= rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::LESS_THAN: {
					result = lvalue->numberData < rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::GREATER_THAN: {
					result = lvalue->numberData > rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::BITWISE_AND: {
					result = (int)lvalue->numberData & (int)rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::BITWISE_OR: {
					result = (int)lvalue->numberData | (int)rvalue->numberData;
					evaluated = true;
					break;
				}

				case instruction::BITWISE_XOR: {
					result = (int)lvalue->numberData ^ (int)rvalue->numberData;
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

		// TODO this whole thing probably needs to be optimized
		case instruction::LOCAL_ASSIGN: {
			Entry* entry;
			if(instruction.localAssign.operation != instruction::EQUALS) {
				entry = &this->getTopVariableContext().getVariableEntry(
					instruction,
					instruction.localAssign.destination
				);
			}

			Entry* value;
			bool fromStack = instruction.localAssign.fromStack;
			if(fromStack) {
				value = &this->stack[this->stackPointer - 1 - instruction.localAssign.dimensions]; // start from top of stack
			}
			else {
				value = &instruction.localAssign.entry;
			}

			switch(instruction.localAssign.operation) {
				case instruction::EQUALS: {
					this->getTopVariableContext().setVariableEntry(
						instruction,
						instruction.localAssign.destination,
						*value
					);
					break;
				}

				case instruction::INCREMENT: {
					entry->numberData++;
					break;
				}

				case instruction::DECREMENT: {
					entry->numberData--;
					break;
				}

				default: {
					break;
				}
			}

			if(instruction.localAssign.fromStack) {
				this->pop(); // pop value from the stack
			}

			for(int i = 0; i < instruction.localAssign.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			break;
		}

		case instruction::LOCAL_ACCESS: { // push local variable to stack
			this->push(
				this->getTopVariableContext().getVariableEntry(
					instruction,
					instruction.localAccess.source
				)
			);

			for(int i = 0; i < instruction.localAccess.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			break;
		}

		case instruction::NEW_FRAME: { // create a new frame
			this->frames[this->framePointer].start = this->stackPointer;
			this->frames[this->framePointer].size = 0;
			this->topFrame = &this->frames[this->framePointer];
			this->framePointer++;
			break;
		}

		case instruction::DELETE_FRAME: { // delete the last frame
			int saveTop = this->topFrame->start + this->topFrame->size;
			int saveStart = saveTop - instruction.deleteFrame.save;

			this->stackPointer -= this->topFrame->size; // pop the contents of the frame
			this->framePointer--; // pop the frame

			// move the saved entries onto the new stack
			for(int i = saveStart; i < saveTop; i++) {
				this->push(this->stack[i]);
			}

			// update top frame pointer
			if(this->framePointer != 0) {
				this->topFrame = &this->frames[this->framePointer - 1];
			}
			else {
				this->topFrame = nullptr;
			}

			break;
		}

		case instruction::PUSH_VARIABLE_CONTEXT: {
			this->pushVariableContext();
			break;
		}

		case instruction::POP_VARIABLE_CONTEXT: {
			this->popVariableContext();
			break;
		}

		case instruction::CALL_FUNCTION: {
			if(!instruction.callFunction.isCached) {
				instruction.callFunction.cachedIndex = this->nameToIndex[instruction.callFunction.name];
				instruction.callFunction.isCached = true;
			}

			this->pushInstructionContainer(this->indexToFunction[instruction.callFunction.cachedIndex]);

			break;
		}

		case instruction::RETURN: {
			this->popInstructionContainer();
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

void Interpreter::addFunction(string &name, InstructionReturn output) {
	auto index = this->functionCount;
	InstructionContainer* container = new InstructionContainer(output.first);
	this->nameToIndex[string(name)] = index;
	this->indexToFunction[index] = container;
}