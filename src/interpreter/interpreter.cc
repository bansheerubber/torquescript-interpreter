#include "interpreter.h"

#include "entry.h"
#include "debug.h"
#include "../util/getEmptyString.h"
#include "object.h"
#include "../util/numberToString.h"
#include "stack.h"
#include "../util/stringCompare.h"
#include "../util/stringToNumber.h"
#include "../util/toLower.h"

using namespace ts;

void ts::initFunctionFrame(Interpreter* interpreter, FunctionFrame* frame) {
	*frame = (FunctionFrame){
		context: VariableContext(interpreter),
		container: nullptr,
		instructionPointer: 0,
		stackPointer: 0,
		stackPopCount: 0,
	};
}

void ts::onFunctionFrameRealloc(Interpreter* interpreter) {
	FunctionFrame &frame = interpreter->frames[interpreter->frames.head - 1];
	interpreter->instructionPointer = &frame.instructionPointer;
	interpreter->stackFramePointer = frame.stackPointer;
	interpreter->topContext = &frame.context;
}

void ts::initPackagedFunctionList(Interpreter* interpreter, PackagedFunctionList** list) {
	*list = nullptr;
}

Interpreter::Interpreter(ParsedArguments args) {
	this->emptyEntry.setString(getEmptyString());

	this->stack = DynamicArray<Entry, Interpreter>(this, 10000, initEntry, nullptr);
	this->frames = DynamicArray<FunctionFrame, Interpreter>(this, 1024, initFunctionFrame, onFunctionFrameRealloc);
	this->functions = DynamicArray<PackagedFunctionList*, Interpreter>(this, 1024, initPackagedFunctionList, nullptr);

	for(sl::Function* function: sl::functions) {
		this->defineTSSLFunction(function);
	}

	if(args.arguments["no-warnings"] != "") {
		this->warnings = false;
	}

	this->globalContext = VariableContext(this);
}

Interpreter::~Interpreter() {
	free(this->stack.array);	
	free(this->frames.array);	
}

void Interpreter::defineTSSLFunction(sl::Function* function) {
	Function* container = new Function(function);
	
	if(function->nameSpace.length() == 0) {
		PackagedFunctionList* list;
		if(this->nameToFunctionIndex.find(toLower(function->name)) == this->nameToFunctionIndex.end()) {
			// add the function to the function-specific datastructure
			this->nameToFunctionIndex[toLower(function->name)] = this->functions.head;
			list = new PackagedFunctionList(function->name);
			list->isTSSL = true;
			this->functions[this->functions.head] = list;
			this->functions.pushed();
		}
		else {
			list = this->functions[this->nameToFunctionIndex[toLower(function->name)]];
		}

		// create the packaged function list
		list->addPackageFunction(container);
	}
	else {
		/*if(this->namespaceToIndex.find(toLower(function->nameSpace)) == this->namespaceToIndex.end()) {
			this->namespaceToIndex[toLower(function->nameSpace)] = this->namespaceFunctions.size();

			// add to function data structure
			NamespaceFunctions* functions = new NamespaceFunctions();
			functions->nameToIndex[toLower(function->name)] = functions->functions.size();
			functions->nameToFunction[toLower(function->name)] = container;
			functions->functions.push_back(container);
			this->namespaceFunctions.push_back(functions);
		}
		else {
			NamespaceFunctions* functions = this->namespaceFunctions[this->namespaceToIndex[function->nameSpace]];
			functions->nameToIndex[toLower(function->name)] = functions->functions.size();
			functions->nameToFunction[toLower(function->name)] = container;
			functions->functions.push_back(container);
		}*/
	}
}

void Interpreter::pushInstructionContainer(
	InstructionContainer* container,
	PackagedFunctionList* list,
	int packagedFunctionListIndex,
	size_t argumentCount,
	size_t popCount
) {
	FunctionFrame &frame = this->frames[this->frames.head];
	frame.container = container;
	frame.instructionPointer = 0;
	frame.stackPointer = this->stack.head - argumentCount;
	frame.stackPopCount = popCount;
	frame.packagedFunctionList = list;
	frame.packagedFunctionListIndex = packagedFunctionListIndex;

	this->topContainer = frame.container;
	this->instructionPointer = &frame.instructionPointer;
	this->stackFramePointer = frame.stackPointer;
	this->topContext = &frame.context;
	
	this->frames.pushed();
}

void Interpreter::popInstructionContainer() {
	this->frames.popped();

	FunctionFrame &frame = this->frames[this->frames.head - 1];
	this->topContainer = frame.container;
	this->instructionPointer = &frame.instructionPointer;
	this->stackFramePointer = frame.stackPointer;
	this->topContext = &frame.context;

	for(size_t i = 0; i < this->frames[this->frames.head].stackPopCount; i++) {
		this->pop();
	}
}

// push an entry onto the stack
void Interpreter::push(Entry &entry) {
	copyEntry(entry, this->stack[this->stack.head]);
	this->stack.pushed();
}

// push a number onto the stack
void Interpreter::push(double number) {
	// manually inline this b/c for some reason it doesn't want to by itself
	this->stack[this->stack.head].type = entry::NUMBER;
	this->stack[this->stack.head].numberData = number;
	this->stack.pushed();
}

// push a string onto the stack
void Interpreter::push(char* value) {
	this->stack[this->stack.head].setString(value);
	this->stack.pushed();
}

// push an object onto the stack
void Interpreter::push(ObjectReference* value) {
	this->stack[this->stack.head].setObject(value);
	this->stack.pushed();
}

void Interpreter::pop() {
	Entry &test = this->stack[this->stack.head - 1];
	if(test.type == entry::STRING && test.stringData) {
		delete test.stringData;
		test.stringData = nullptr;
	}
	
	this->stack.popped();
}

void Interpreter::startInterpretation(Instruction* head) {
	this->pushInstructionContainer(new InstructionContainer(head)); // create the instructions
	this->startTime = chrono::high_resolution_clock::now();
	this->interpret();
}

void Interpreter::warning(const char* format, ...) {
	if(this->warnings) {
		va_list argptr;
		va_start(argptr, format);
		printWarning(format, argptr);
		va_end(argptr);
	}
}

void Interpreter::interpret() {
	start:
	Instruction &instruction = this->topContainer->array[*this->instructionPointer];
	
	if(*this->instructionPointer >= this->topContainer->size) { // quit once we run out of instructions
		if(!this->testing) {
			size_t elapsed = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->startTime)).count();
			printf("ran in %lu us\n", elapsed);
			this->topContext->print();
			this->printStack();
		}
		return;
	}

	(*this->instructionPointer)++;

	// PrintInstruction(instruction);
	
	switch(instruction.type) {
		case instruction::INVALID_INSTRUCTION: {
			printError("invalid instruction\n");
			exit(1);
		}

		// generate code for math instructions
		## math_generator.py

		// generate code for assignment instructions
		## assignments_generator.py
		
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
			Entry &entry = this->stack[this->stack.head - 1];
			if(
				(
					entry.type == entry::NUMBER
					&& entry.numberData != 0
				)
				|| (
					entry.type == entry::STRING
					&& strlen(entry.stringData) != 0
				)
				|| (
					entry.type == entry::OBJECT
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
			Entry &entry = this->stack[this->stack.head - 1];
			if(
				(
					entry.type == entry::NUMBER
					&& entry.numberData == 0
				)
				|| (
					entry.type == entry::STRING
					&& strlen(entry.stringData) == 0
				)
			) {
				*this->instructionPointer = instruction.jumpIfFalse.index;
			}

			if(instruction.jumpIfFalse.pop) {
				this->pop();
			}
			break;
		}

		case instruction::UNARY_MATHEMATICS: {
			Entry &value = this->stack[this->stack.head - 1];
			double valueNumber = 0;
			if(value.type == entry::NUMBER) {
				valueNumber = value.numberData;
			}
			else {
				valueNumber = stringToNumber(value.stringData);
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
		
		case instruction::LOCAL_ACCESS: { // push local variable to stack
			if(instruction.localAccess.stackIndex < 0) {
				Entry &entry = this->topContext->getVariableEntry(
					instruction,
					instruction.localAccess.source,
					instruction.localAccess.hash
				);

				for(int i = 0; i < instruction.localAccess.dimensions; i++) {
					this->pop(); // pop the dimensions if we have any
				}

				this->push(entry);	
			}
			else {
				this->push(this->stack[instruction.localAccess.stackIndex + this->stackFramePointer]);
			}

			break;
		}

		case instruction::GLOBAL_ACCESS: { // push local variable to stack
			Entry &entry = this->globalContext.getVariableEntry(
				instruction,
				instruction.globalAccess.source,
				instruction.globalAccess.hash
			);

			for(int i = 0; i < instruction.globalAccess.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			this->push(entry);

			break;
		}

		case instruction::OBJECT_ACCESS: { // push object property to stack
			Entry &objectEntry = this->stack[this->stack.head - 1 - instruction.localAssign.dimensions];
			ObjectReference* object = objectEntry.objectData;

			// if the object is not alive anymore, push nothing to the stack
			if(objectEntry.type != entry::OBJECT || object->object == nullptr) {
				this->pop(); // pop the object
				this->push(this->emptyEntry);

				this->warning("trying to access deleted object\n");
				break;
			}
			
			Entry &entry = object->object->properties.getVariableEntry(
				instruction,
				instruction.localAccess.source,
				instruction.localAssign.hash
			);

			for(int i = 0; i < instruction.localAccess.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			this->pop(); // pop the object

			this->push(entry);

			break;
		}

		case instruction::CALL_FUNCTION: { // jump to a new instruction container
			if(!instruction.callFunction.isCached) {
				bool found = false;
				if(
					instruction.callFunction.nameSpace.length() != 0
					&& this->namespaceToIndex.find(toLower(instruction.callFunction.nameSpace)) != this->namespaceToIndex.end()
				) {
					/*int namespaceIndex = this->namespaceToIndex[toLower(instruction.callFunction.nameSpace)];
					if(
						this->namespaceFunctions[namespaceIndex]->nameToIndex.find(toLower(instruction.callFunction.name))
							!= this->namespaceFunctions[namespaceIndex]->nameToIndex.end()
					) {
						instruction.callFunction.cachedIndex = this->namespaceFunctions[namespaceIndex]->nameToIndex[toLower(instruction.callFunction.name)];
						instruction.callFunction.cachedNamespaceIndex = namespaceIndex;
						instruction.callFunction.isCached = true;
						instruction.callFunction.isNamespaceCached = true;

						if(this->namespaceFunctions[namespaceIndex]->functions[instruction.callFunction.cachedIndex]->isTSSL) {
							instruction.callFunction.isTSSL = true;
						}

						found = true;
					}*/
				}
				else { // find non-namespace function
					if(this->nameToFunctionIndex.find(toLower(instruction.callFunction.name)) != this->nameToFunctionIndex.end()) {
						instruction.callFunction.cachedIndex = this->nameToFunctionIndex[toLower(instruction.callFunction.name)];
						instruction.callFunction.isCached = true;

						if(this->functions[instruction.callFunction.cachedIndex]->isTSSL) {
							instruction.callFunction.isTSSL = true;
						}

						found = true;
					}
				}

				// print warning if function was not defined
				if(found == false) {
					this->warning("could not find function with name '%s'\n", instruction.callFunction.name.c_str());
				
					// pop arguments that we didn't use
					Entry &numberOfArguments = this->stack[this->stack.head - 1];
					int number = (int)numberOfArguments.numberData;
					for(int i = 0; i < number + 1; i++) {
						this->pop();
					}

					this->push(this->emptyEntry);
					break;
				}
			}

			Function* foundFunction;
			PackagedFunctionList* list;
			int packagedFunctionListIndex = -1;
			if(instruction.callFunction.isNamespaceCached) {
				foundFunction = this->namespaceFunctions[instruction.callFunction.cachedNamespaceIndex]->functions[instruction.callFunction.cachedIndex];
			}
			else {
				list = this->functions[instruction.callFunction.cachedIndex];
				packagedFunctionListIndex = list->topValidIndex;
				foundFunction = (*list)[list->topValidIndex];
			}

			## call_generator.py

			break;
		}

		case instruction::RETURN: { // return from a function
			copyEntry(this->stack[this->stack.head - 1], this->returnRegister);
			this->pop(); // pop return value
			this->popInstructionContainer();
			this->push(this->returnRegister); // push return register
			break;
		}

		case instruction::POP_ARGUMENTS: {
			Entry &numberOfArguments = this->stack[this->stack.head - 1];
			size_t realNumberOfArguments = instruction.popArguments.argumentCount;
			int number = (int)numberOfArguments.numberData - realNumberOfArguments;

			this->pop(); // pop argument count
			for(int i = 0; i < number; i++) {
				this->pop();
			}

			for(int i = 0; i < -number; i++) {
				this->push(getEmptyString());
			}

			break;
		}

		case instruction::CREATE_OBJECT: {
			if(
				!instruction.createObject.isCached
				&& this->namespaceToIndex.find(instruction.createObject.type) != this->namespaceToIndex.end()
			) {
				instruction.createObject.namespaceIndex = this->namespaceToIndex[instruction.createObject.type];
				instruction.createObject.isCached = true;
			}
			
			Object* object = new Object(this, instruction.createObject.type, instruction.createObject.namespaceIndex);
			this->push(new ObjectReference(object));
			break;
		}

		case instruction::CALL_OBJECT: {
			printf("no calling objects\n");
			exit(1);
			
			PackagedFunctionList* list;
			int packagedFunctionListIndex = -1;
			Entry &numberOfArguments = this->stack[this->stack.head - 1];
			int argumentCount = (int)numberOfArguments.numberData;
			
			// pull the object from the stack
			Entry &objectEntry = this->stack[this->stack.head - 1 - argumentCount];
			ObjectReference* object = objectEntry.objectData;

			if(objectEntry.type != entry::OBJECT || object->object == nullptr) {
				this->warning("trying to call a deleted object\n");
				
				// pop arguments that we didn't use
				Entry &numberOfArguments = this->stack[this->stack.head - 1];
				int number = (int)numberOfArguments.numberData;
				for(int i = 0; i < number + 1; i++) {
					this->pop();
				}

				this->push(this->emptyEntry);
				break;
			}

			auto search = this->namespaceFunctions[object->object->namespaceIndex]->nameToFunction.find(toLower(instruction.callObject.name));
			if(search != this->namespaceFunctions[object->object->namespaceIndex]->nameToFunction.end()) {
				Function* foundFunction = search->second;
				## call_generator.py
			}
			else {
				this->warning("could not find function with name '%s::%s'\n", object->object->nameSpace.c_str(), instruction.callFunction.name.c_str());

				// pop arguments that we didn't use
				Entry &numberOfArguments = this->stack[this->stack.head - 1];
				int number = (int)numberOfArguments.numberData;
				for(int i = 0; i < number + 1; i++) {
					this->pop();
				}

				this->push(this->emptyEntry);
			}
			
			break;
		}

		case instruction::LINK_VARIABLE: {
			this->topContext->linkVariable(instruction.linkVariable.source, instruction.linkVariable.hash, instruction.linkVariable.stackIndex);
			break;
		}

		case instruction::CALL_PARENT: {
			FunctionFrame &frame = this->frames[this->frames.head - 1];
			PackagedFunctionList* list = frame.packagedFunctionList;
			int packagedFunctionListIndex = frame.packagedFunctionList->getNextValidIndex(frame.packagedFunctionListIndex);

			if(packagedFunctionListIndex != -1) {
				Function* foundFunction = (*list)[packagedFunctionListIndex];
				## call_generator.py
			}
			else {
				printError("failed to do parent\n");
				exit(1);
			}

			break;
		}

		default: {
			printf("DID NOT EXECUTE INSTRUCTION.\n");
		}
	}

	// this->printStack();

	goto start;
}

void Interpreter::printStack() {
	printf("\nSTACK: %ld\n", this->stack.head);
	for(size_t i = 0; i < this->stack.head; i++) {
		Entry &entry = this->stack[i];

		printf("#%ld ", i);
		entry.print();
	}
	printf("\n");
}

void Interpreter::defineFunction(string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
	// create the function container which we will use to execute the function at runtime
	Function* container = new Function(output.first, argumentCount, variableCount, name);
	
	PackagedFunctionList* list;
	if(this->nameToFunctionIndex.find(toLower(name)) == this->nameToFunctionIndex.end()) {
		// add the function to the function-specific datastructure
		this->nameToFunctionIndex[toLower(name)] = this->functions.head;
		list = new PackagedFunctionList(name);
		this->functions[this->functions.head] = list;
		this->functions.pushed();
	}
	else {
		list = this->functions[this->nameToFunctionIndex[toLower(name)]];
	}

	// create the packaged function list
	list->addInitialFunction(container);
}

void Interpreter::addFunction(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
	/*Function* container = new Function(output.first, argumentCount, variableCount, name, nameSpace);

	if(this->namespaceToIndex.find(toLower(nameSpace)) == this->namespaceToIndex.end()) {
		this->namespaceToIndex[toLower(nameSpace)] = this->namespaceFunctions.size();

		// add to function data structure
		NamespaceFunctions* functions = new NamespaceFunctions();
		functions->nameToIndex[toLower(name)] = functions->functions.size();
		functions->nameToFunction[toLower(name)] = container;
		functions->functions.push_back(container);
		this->namespaceFunctions.push_back(functions);
	}
	else {
		NamespaceFunctions* functions = this->namespaceFunctions[this->namespaceToIndex[nameSpace]];
		functions->nameToIndex[toLower(name)] = functions->functions.size();
		functions->nameToFunction[toLower(name)] = container;
		functions->functions.push_back(container);
	}*/
}

void Interpreter::addPackageFunction(Package* package, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
	// create the function container which we will use to execute the function at runtime
	Function* container = new Function(output.first, argumentCount, variableCount, name);
	
	PackagedFunctionList* list;
	if(this->nameToFunctionIndex.find(toLower(name)) == this->nameToFunctionIndex.end()) {
		// add the function to the function-specific datastructure
		this->nameToFunctionIndex[toLower(name)] = this->functions.head;
		list = new PackagedFunctionList(name);
		this->functions[this->functions.head] = list;
		this->functions.pushed();
	}
	else {
		list = this->functions[this->nameToFunctionIndex[toLower(name)]];
	}

	// create the packaged function list
	list->addPackageFunction(container);
}