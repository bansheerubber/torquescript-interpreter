#include "interpreter.h"
#include "entry.h"
#include "debug.h"
#include "../util/stringToNumber.h"
#include "../util/numberToString.h"
#include "../util/toLower.h"

using namespace ts;

Interpreter::Interpreter() {
	this->pushVariableContext();
}

Interpreter::~Interpreter() {
	
}

void Interpreter::pushInstructionContainer(InstructionContainer* container) {
	// push container
	this->topContainer = container;
	this->containerStack[this->containerStackPointer] = container;
	this->containerStackPointer++;

	// push pointer
	this->pointerStack[this->pointerStackPointer] = 0;
	this->instructionPointer = &this->pointerStack[this->pointerStackPointer];
	this->pointerStackPointer++;
}

void Interpreter::popInstructionContainer() {
	// pop container
	this->containerStackPointer--;
	this->topContainer = this->containerStack[this->containerStackPointer - 1];

	// pop pointer
	this->pointerStackPointer--;
	this->instructionPointer = &this->pointerStack[this->pointerStackPointer - 1];
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

// push a string onto the stack
void Interpreter::push(string* value) {
	if(this->framePointer != 0) {
		this->topFrame->size++;
	}
	this->stack[this->stackPointer].setString(value);
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

			// begin typechecking
			double lvalueNumber = 0;
			double rvalueNumber = 0;
			string* lvalueString = nullptr;
			string* rvalueString = nullptr;
			bool deleteLValueString = false; // if we need to delete the string if we converted from number to string
			bool deleteRValueString = false;
			if(instruction.mathematics.operation < instruction::STRING_EQUAL) { // if we're dealing with numerical operation
				if(lvalue->type == entry::NUMBER) { // typecheck lvalue
					lvalueNumber = lvalue->numberData;
				}
				else {
					lvalueNumber = stringToNumber(*lvalue->stringData);
				}

				if(rvalue->type == entry::NUMBER) { // typecheck rvalue
					rvalueNumber = rvalue->numberData;
				}
				else {
					rvalueNumber = stringToNumber(*rvalue->stringData);
				}
			}
			else { // if we're dealing with a string operation
				if(lvalue->type == entry::STRING) { // typecheck lvalue
					lvalueString = lvalue->stringData;
				}
				else {
					lvalueString = numberToString(lvalue->numberData);
					deleteLValueString = true;
				}

				if(rvalue->type == entry::STRING) { // typecheck rvalue
					rvalueString = rvalue->stringData;
				}
				else {
					rvalueString = numberToString(rvalue->numberData);
					deleteRValueString = true;
				}
			}

			double numberResult = 0.0;
			string* stringResult = nullptr;
			bool evaluated = true;
			switch(instruction.mathematics.operation) {
				case instruction::ADDITION: {
					numberResult = lvalueNumber + rvalueNumber;
					break;
				}

				case instruction::SUBTRACT: {
					numberResult = lvalueNumber - rvalueNumber;
					break;
				}

				case instruction::DIVISION: {
					numberResult = lvalueNumber / rvalueNumber;
					break;
				}

				case instruction::MULTIPLY: {
					numberResult = lvalueNumber * rvalueNumber;
					break;
				}

				case instruction::EQUAL: {
					numberResult = lvalueNumber == rvalueNumber;
					break;
				}

				case instruction::LESS_THAN_EQUAL: {
					numberResult = lvalueNumber <= rvalueNumber;
					break;
				}

				case instruction::GREATER_THAN_EQUAL: {
					numberResult = lvalueNumber >= rvalueNumber;
					break;
				}

				case instruction::LESS_THAN: {
					numberResult = lvalueNumber < rvalueNumber;
					break;
				}

				case instruction::GREATER_THAN: {
					numberResult = lvalueNumber > rvalueNumber;
					break;
				}

				case instruction::BITWISE_AND: {
					numberResult = (int)lvalueNumber & (int)rvalueNumber;
					break;
				}

				case instruction::BITWISE_OR: {
					numberResult = (int)lvalueNumber | (int)rvalueNumber;
					break;
				}

				case instruction::BITWISE_XOR: {
					numberResult = (int)lvalueNumber ^ (int)rvalueNumber;
					break;
				}

				case instruction::STRING_EQUAL: {
					numberResult = toLower(*lvalueString).compare(toLower(*rvalueString)) == 0;
					break;
				}

				case instruction::STRING_NOT_EQUAL: {
					numberResult = toLower(*lvalueString).compare(toLower(*rvalueString)) != 0;
					break;
				}

				case instruction::APPEND: { // append data to lvalue
					stringResult = new string(*lvalueString);
					*stringResult += *rvalueString;
					break;
				}

				case instruction::SPC: { // append data to lvalue
					stringResult = new string(*lvalueString);
					*stringResult += ' ' + *rvalueString;
					break;
				}

				default: {
					evaluated = false;
					break;
				}
			}

			if(deleteLValueString) {
				delete lvalueString;
			}

			if(deleteRValueString) {
				delete rvalueString;
			}
			
			if(evaluated) {
				if(stringResult == nullptr) {
					this->push(numberResult);
				}
				else {
					this->push(stringResult);
				}
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

		case instruction::CALL_FUNCTION: {
			if(!instruction.callFunction.isCached) {
				instruction.callFunction.cachedIndex = this->nameToIndex[instruction.callFunction.name];
				instruction.callFunction.isCached = true;
			}

			this->pushInstructionContainer(this->indexToFunction[instruction.callFunction.cachedIndex]);
			this->pushVariableContext();
			break;
		}

		case instruction::RETURN: {
			this->popInstructionContainer();
			this->popVariableContext();
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