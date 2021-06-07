#include "interpreter.h"
#include "entry.h"
#include "debug.h"
#include "../util/stringToNumber.h"
#include "../util/numberToString.h"

using namespace ts;

Interpreter::Interpreter() {
	for(int i = 0; i < 256; i++) {
		this->contexts[i].interpreter = this;
	}
	
	this->pushVariableContext();
	this->emptyEntry.setString(new string(""));
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
	this->topContext = &this->contexts[this->contextPointer];
	this->contextPointer++;
}

void Interpreter::popVariableContext() {
	this->topContext->clear();
	this->contextPointer--;
	this->topContext = &this->contexts[this->contextPointer - 1];
}

// push an entry onto the stack
void Interpreter::push(Entry &entry) {
	copyEntry(entry, this->stack[this->stackPointer]);
	this->stackPointer++;
}

// push a number onto the stack
void Interpreter::push(double number) {
	this->stack[this->stackPointer].setNumber(number);
	this->stackPointer++;
}

// push a string onto the stack
void Interpreter::push(string* value) {
	this->stack[this->stackPointer].setString(value);
	this->stackPointer++;
}

void Interpreter::pop() {
	if(this->stackPointer == 0) { // protect against empty stack
		printError("empty stack\n");
		exit(1);
	}
	
	this->stackPointer--;
}

void Interpreter::startInterpretation(Instruction* head) {
	this->pushInstructionContainer(new InstructionContainer(head)); // create the instructions
	// this->topContainer->print();
	this->startTime = chrono::high_resolution_clock::now();
	this->interpret();
}

void Interpreter::interpret() {
	if(*this->instructionPointer >= this->topContainer->size) { // quit once we run out of instructions
		long int elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->startTime)).count();
		printf("ran %d instructions in %lu us\n", this->ranInstructions, elapsed);
		this->topContext->print();
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
			Entry &entry = this->stack[this->stackPointer - 1];
			if(
				(
					entry.type == entry::NUMBER
					&& entry.numberData != 0
				)
				|| (
					entry.type == entry::STRING
					&& entry.stringData->length() != 0
				)
			) {
				*this->instructionPointer = instruction.jumpIfTrue.index;
			}

			if(instruction.jumpIfTrue.pop) {
				this->pop();
			}
			break;
		}

		case instruction::JUMP_IF_FALSE: { // jump to an instruction
			Entry &entry = this->stack[this->stackPointer - 1];
			if(
				(
					entry.type == entry::NUMBER
					&& entry.numberData == 0
				)
				|| (
					entry.type == entry::STRING
					&& entry.stringData->length() == 0
				)
			) {
				*this->instructionPointer = instruction.jumpIfFalse.index;
			}

			if(instruction.jumpIfFalse.pop) {
				this->pop();
			}
			break;
		}

		case instruction::MATH_ADDITION: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber + rvalueNumber);
			break;
		}

		case instruction::MATH_SUBTRACT: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber - rvalueNumber);
			break;
		}

		case instruction::MATH_MULTIPLY: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber * rvalueNumber);
			break;
		}

		case instruction::MATH_DIVISION: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber / rvalueNumber);
			break;
		}

		case instruction::MATH_MODULUS: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push((int)lvalueNumber % (int)rvalueNumber);
			break;
		}

		case instruction::MATH_SHIFT_LEFT: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push((int)lvalueNumber << (int)rvalueNumber);
			break;
		}

		case instruction::MATH_SHIFT_RIGHT: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push((int)lvalueNumber >> (int)rvalueNumber);
			break;
		}

		case instruction::MATH_EQUAL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber == rvalueNumber);
			break;
		}

		case instruction::MATH_LESS_THAN_EQUAL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber <= rvalueNumber);
			break;
		}

		case instruction::MATH_GREATER_THAN_EQUAL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber >= rvalueNumber);
			break;
		}

		case instruction::MATH_LESS_THAN: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber < rvalueNumber);
			break;
		}

		case instruction::MATH_GREATER_THAN: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push(lvalueNumber > rvalueNumber);
			break;
		}

		case instruction::MATH_BITWISE_AND: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push((int)lvalueNumber & (int)rvalueNumber);
			break;
		}

		case instruction::MATH_BITWISE_OR: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif
			
			this->push((int)lvalueNumber | (int)rvalueNumber);
			break;
		}

		case instruction::MATH_BITWISE_XOR: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
			#endif

			this->push((int)lvalueNumber ^ (int)rvalueNumber);
			break;
		}

		case instruction::MATH_STRING_EQUAL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			this->push(toLower(*lvalueString).compare(toLower(*rvalueString)) == 0);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::MATH_STRING_NOT_EQUAL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			this->push(toLower(*lvalueString).compare(toLower(*rvalueString)) != 0);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::MATH_APPEND: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			string* stringResult = new string(*lvalueString);
			*stringResult += *rvalueString;
			this->push(stringResult);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::MATH_SPC: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			string* stringResult = new string(*lvalueString);
			*stringResult += ' ' + *rvalueString;
			this->push(stringResult);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::MATH_TAB: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			string* stringResult = new string(*lvalueString);
			*stringResult += '\t' + *rvalueString;
			this->push(stringResult);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::MATH_NL: {
			#if TS_INTERPRETER_PREFIX
				TS_MATH_DEFINE_STRING_VALUES_PREFIX();
			#else
				TS_MATH_DEFINE_STRING_VALUES_POSTFIX();
			#endif

			string* stringResult = new string(*lvalueString);
			*stringResult += '\n' + *rvalueString;
			this->push(stringResult);

			// delete lvalue string if we converted it from a number
			if(deleteLValueString) {
				delete lvalueString;
			}

			// delete rvalue string if we converted it from a number
			if(deleteRValueString) {
				delete rvalueString;
			}

			break;
		}

		case instruction::UNARY_MATHEMATICS: {
			Entry &value = this->stack[this->stackPointer - 1];
			double valueNumber = 0;
			if(value.type == entry::NUMBER) {
				valueNumber = value.numberData;
			}
			else {
				valueNumber = stringToNumber(*value.stringData);
			}
			
			this->pop();

			double result = 0.0;
			switch(instruction.unaryMathematics.operation) {
				case instruction::BITWISE_NOT: {
					result = ~((int)valueNumber);
					break;
				}

				case instruction::LOGICAL_NOT: {
					result = !((int)valueNumber);
					break;
				}

				case instruction::NEGATE: {
					result = -((int)valueNumber);
					break;
				}
			}
			this->push(result);
			break;
		}

		case instruction::ARGUMENT_ASSIGN: { // assign argument a value from the stack
			int actualArgumentCount = (int)this->stack[this->stackPointer - 1].numberData; // get the amount of arguments used
			int delta = instruction.argumentAssign.argc - actualArgumentCount;
			relative_stack_location location = this->stackPointer - 1 - instruction.argumentAssign.offset + delta;

			if((int)instruction.argumentAssign.offset <= delta) {
				this->topContext->setVariableEntry(
					instruction,
					instruction.argumentAssign.destination,
					instruction.argumentAssign.hash,
					this->emptyEntry
				);
			}
			else {
				this->topContext->setVariableEntry(
					instruction,
					instruction.argumentAssign.destination,
					instruction.argumentAssign.hash,
					this->stack[location]
				);
			}
			break;
		}

		// TODO this whole thing probably needs to be optimized
		case instruction::LOCAL_ASSIGN: {
			Entry* entry = nullptr;
			double entryNumber = 0;
			string* entryString = nullptr;
			if(instruction.localAssign.operation != instruction::EQUALS) {
				entry = &this->topContext->getVariableEntry(
					instruction,
					instruction.localAssign.destination,
					instruction.localAssign.hash
				);

				if(entry->type == entry::NUMBER) {
					entryNumber = entry->numberData;
				}
				else {
					entryNumber = stringToNumber(*entry->stringData);
				}
			}

			Entry* value = nullptr;
			bool fromStack = instruction.localAssign.fromStack;
			if(fromStack) {
				value = &this->stack[this->stackPointer - 1 - instruction.localAssign.dimensions]; // start from top of stack
			}
			else {
				value = &instruction.localAssign.entry;
			}

			double valueNumber = 0;
			if(instruction.localAssign.operation >= instruction::PLUS_EQUALS) {
				if(value->type == entry::NUMBER) {
					valueNumber = value->numberData;
				}
				else {
					valueNumber = stringToNumber(*value->stringData);
				}
			}

			switch(instruction.localAssign.operation) {
				case instruction::EQUALS: {
					this->topContext->setVariableEntry(
						instruction,
						instruction.localAssign.destination,
						instruction.localAssign.hash,
						*value
					);
					break;
				}

				case instruction::INCREMENT: {
					entry->setNumber(++entryNumber);
					break;
				}

				case instruction::DECREMENT: {
					entry->setNumber(--entryNumber);
					break;
				}

				case instruction::PLUS_EQUALS: {
					entry->setNumber(entryNumber + valueNumber);
					break;
				}

				case instruction::MINUS_EQUALS: {
					entry->setNumber(entryNumber - valueNumber);
					break;
				}

				case instruction::ASTERISK_EQUALS: {
					entry->setNumber(entryNumber * valueNumber);
					break;
				}

				case instruction::SLASH_EQUALS: {
					entry->setNumber(entryNumber / valueNumber);
					break;
				}

				case instruction::MODULUS_EQUALS: {
					entry->setNumber((int)entryNumber % (int)valueNumber);
					break;
				}

				case instruction::SHIFT_LEFT_EQUALS: {
					entry->setNumber((int)entryNumber << (int)valueNumber);
					break;
				}

				case instruction::SHIFT_RIGHT_EQUALS: {
					entry->setNumber((int)entryNumber >> (int)valueNumber);
					break;
				}

				case instruction::BITWISE_AND_EQUALS: {
					entry->setNumber((int)entryNumber & (int)valueNumber);
					break;
				}

				case instruction::BITWISE_XOR_EQUALS: {
					entry->setNumber((int)entryNumber ^ (int)valueNumber);
					break;
				}

				case instruction::BITWISE_OR_EQUALS: {
					entry->setNumber((int)entryNumber | (int)valueNumber);
					break;
				}

				default: {
					break;
				}
			}

			for(int i = 0; i < instruction.localAssign.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			if(instruction.localAssign.fromStack) {
				this->pop(); // pop value from the stack
			}

			if(instruction.localAssign.operation != instruction::EQUALS) {
				if(instruction.localAssign.pushResult && entry != nullptr) {
					this->push(*entry);
				}
			}
			else if(instruction.localAssign.pushResult) {
				this->push(*value);
			}

			break;
		}

		case instruction::LOCAL_ACCESS: { // push local variable to stack
			Entry &entry = this->topContext->getVariableEntry(
				instruction,
				instruction.localAccess.source,
				instruction.localAssign.hash
			);

			for(int i = 0; i < instruction.localAccess.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			this->push(entry);

			break;
		}

		case instruction::CALL_FUNCTION: { // jump to a new instruction container
			if(!instruction.callFunction.isCached) {
				bool found = false;
				if(this->nameToIndex.find(toLower(instruction.callFunction.name)) != this->nameToIndex.end()) {
					instruction.callFunction.cachedIndex = this->nameToIndex[toLower(instruction.callFunction.name)];
					instruction.callFunction.isCached = true;
					found = true;
				}

				if(functions::nameToIndex.find(toLower(instruction.callFunction.name)) != functions::nameToIndex.end()) {
					instruction.callFunction.cachedIndex = functions::nameToIndex[toLower(instruction.callFunction.name)];
					instruction.callFunction.isCached = true;
					instruction.callFunction.isTSSL = true;
					found = true;
				}

				if(found == false) {
					printError("could not find function with name '%s'\n", instruction.callFunction.name.c_str());
					exit(1);
				}
			}

			if(instruction.callFunction.isTSSL) { // call a standard library function if this instruction is defined as such
				functions::Function* function = functions::functions[instruction.callFunction.cachedIndex];

				void* arguments[TS_ARG_COUNT];
				bool deleteArguments[TS_ARG_COUNT];
				// copy the same logic from ARGUMENT_ASSIGN instruction for consistency
				int actualArgumentCount = (int)this->stack[this->stackPointer - 1].numberData; // get the amount of arguments used in the program
				int delta = function->argumentCount - actualArgumentCount;

				int count = 1;
				for(int i = function->argumentCount; i >= 1; i--) { // load arguments array backwards
					if(count <= delta) { // TODO better error handling
						break;
					}
					else {
						relative_stack_location location = this->stackPointer - 1 - i + delta;
						Entry &entry = this->stack[location];
						if(function->argumentTypes[count - 1] == functions::STRING) {
							if(entry.type == entry::NUMBER) {
								arguments[count - 1] = numberToString(entry.numberData);
								deleteArguments[count - 1] = true;
							}
							else {
								arguments[count - 1] = entry.stringData;
								deleteArguments[count - 1] = false;
							}
						}
						else {
							if(entry.type == entry::NUMBER) {
								arguments[count - 1] = &entry.numberData;
								deleteArguments[count - 1] = false;
							}
							else {
								arguments[count - 1] = new double(stringToNumber(*entry.stringData));
								deleteArguments[count - 1] = true;
							}
						}
					}
					count++;
				}

				// pop the data
				for(int i = 0; i < actualArgumentCount + 1; i++) {
					this->pop();
				}

				void* returnValue = function->function(actualArgumentCount, arguments);
				if(function->returnType == functions::type::STRING) {
					this->push((string*)returnValue);
				}
				else if(function->returnType == functions::type::NUMBER) {
					this->push(*((double*)returnValue));
					delete (double*)returnValue;
				}
				else { // push void return value
					this->push(this->emptyEntry);
				}

				for(int i = 0; i < count - 1; i++) {
					if(deleteArguments[i]) {
						delete arguments[i];
					}
				}
			}
			else {
				this->pushInstructionContainer(this->indexToFunction[instruction.callFunction.cachedIndex]);
				this->pushVariableContext();
			}
			break;
		}

		case instruction::RETURN: { // return from a function
			this->popInstructionContainer();
			this->popVariableContext();
			break;
		}

		case instruction::POP_ARGUMENTS: {
			Entry &numberOfArguments = this->stack[this->stackPointer - 1];

			int number = (int)numberOfArguments.numberData;
			for(int i = 0; i < number + 1; i++) {
				this->pop();
			}

			break;
		}
	}

	// this->printStack();

	this->ranInstructions++;

	this->interpret();
}

void Interpreter::printStack() {
	printf("\nSTACK: %d\n", this->stackPointer);
	for(unsigned int i = 0; i < this->stackPointer; i++) {
		Entry &entry = this->stack[i];
		
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
	this->nameToIndex[toLower(name)] = index;
	this->indexToFunction[index] = container;
}