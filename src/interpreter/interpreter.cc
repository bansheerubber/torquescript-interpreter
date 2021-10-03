#include "interpreter.h"

#include "../util/cloneString.h"
#include "../compiler/compiler.h"
#include "debug.h"
#include "../tssl/define.h"
#include "../engine/engine.h"
#include "entry.h"
#include "../util/getEmptyString.h"
#include "../util/isInteger.h"
#include "object.h"
#include "../util/numberToString.h"
#include "../parser/parser.h"
#include "stack.h"
#include "../util/stringCompare.h"
#include "../util/stringToNumber.h"
#include "../util/time.h"
#include "../tokenizer/tokenizer.h"
#include "../util/toLower.h"

using namespace ts;

void ts::initFunctionFrame(Interpreter* interpreter, FunctionFrame* frame) {
	frame->context = new VariableContext(interpreter);
	frame->container = nullptr;
	frame->instructionPointer = 0;
	frame->stackPointer = 0;
	frame->stackPopCount = 0;
}

void ts::onFunctionFrameRealloc(Interpreter* interpreter) {
	FunctionFrame &frame = interpreter->frames[interpreter->frames.head - 1];
	interpreter->instructionPointer = &frame.instructionPointer;
	interpreter->stackFramePointer = frame.stackPointer;
	interpreter->topContext = frame.context;
}

void ts::initSchedule(Interpreter* interpreter, Schedule** schedule) {
	*schedule = nullptr;
}

Interpreter::Interpreter(Engine* engine, ParsedArguments args, bool isParallel) {
	this->engine = engine;
	
	this->emptyEntry.setString(getEmptyString());

	if(args.arguments["no-warnings"] != "") {
		this->warnings = false;
	}

	if(args.arguments["time"] != "") {
		this->showTime = true;
	}

	this->globalContext = VariableContext(this);

	if(this->isParallel) {
		this->enterParallel();
		this->startTime = getMicrosecondsNow();
	}
}

Interpreter::~Interpreter() {
	for(size_t i = 0; i < this->frames.size; i++) {
		delete this->frames[i].context;
	}
}

void Interpreter::enterParallel() { 
	this->tickThread = thread(&Interpreter::tick, this);
	this->isParallel = true;
}

void Interpreter::pushFunctionFrame(
	InstructionContainer* container,
	PackagedFunctionList* list,
	int packagedFunctionListIndex,
	MethodTreeEntry* methodTreeEntry,
	int methodTreeEntryIndex,
	size_t argumentCount,
	size_t popCount
) {
	if(this->frames.head == 0) {
		this->startTime = getMicrosecondsNow();
	}
	
	FunctionFrame &frame = this->frames[this->frames.head];
	frame.container = container;
	frame.instructionPointer = 0;
	frame.stackPointer = this->stack.head - argumentCount;
	frame.stackPopCount = popCount;
	frame.packagedFunctionList = list;
	frame.packagedFunctionListIndex = packagedFunctionListIndex;
	frame.methodTreeEntry = methodTreeEntry;
	frame.methodTreeEntryIndex = methodTreeEntryIndex;
	frame.isTSSL = false;

	this->topContainer = frame.container;
	this->instructionPointer = &frame.instructionPointer;
	this->stackFramePointer = frame.stackPointer;
	this->topContext = frame.context;
	
	this->frames.pushed();
}

void Interpreter::popFunctionFrame() {
	this->frames.popped();

	if(this->frames.head == 0) {
		this->topContainer = nullptr;
		this->instructionPointer = nullptr;
		this->stackFramePointer = 0;
		this->topContext = nullptr;
	}
	else {
		FunctionFrame &frame = this->frames[this->frames.head - 1];
		this->topContainer = frame.container;
		this->instructionPointer = &frame.instructionPointer;
		this->stackFramePointer = frame.stackPointer;
		this->topContext = frame.context;

		for(size_t i = 0; i < this->frames[this->frames.head].stackPopCount; i++) {
			this->pop();
		}
	}
}

void Interpreter::pushTSSLFunctionFrame(MethodTreeEntry* methodTreeEntry, int methodTreeEntryIndex) {
	FunctionFrame &frame = this->frames[this->frames.head];
	frame.isTSSL = true;
	frame.stackPopCount = 0;
	frame.methodTreeEntry = methodTreeEntry;
	frame.methodTreeEntryIndex = methodTreeEntryIndex;
	this->frames.pushed();
}

// push an entry onto the stack
void Interpreter::push(Entry &entry, instruction::PushType type) {
	if(type < 0) {
		copyEntry(entry, this->stack[this->stack.head]);
		this->stack.pushed();
	}
	else {
		copyEntry(entry, this->returnRegister);
	}
}

// push an entry onto the stack, greedily
void Interpreter::push(Entry &entry, instruction::PushType type, bool greedy) {
	if(type < 0) {
		greedyCopyEntry(entry, this->stack[this->stack.head]);
		this->stack.pushed();
	}
	else {
		greedyCopyEntry(entry, this->returnRegister);
	}
}

// push a number onto the stack
void Interpreter::push(double number, instruction::PushType type) {
	if(type < 0) {
		// manually inline this b/c for some reason it doesn't want to by itself
		this->stack[this->stack.head].type = entry::NUMBER;
		this->stack[this->stack.head].numberData = number;
		this->stack.pushed();	
	}
	else {
		this->returnRegister.type = entry::NUMBER;
		this->returnRegister.numberData = number;
	}
}

// push a string onto the stack
void Interpreter::push(char* value, instruction::PushType type) {
	if(type < 0) {
		this->stack[this->stack.head].setString(value);
		this->stack.pushed();
	}
	else {
		this->returnRegister.setString(value);
	}
}

// push an object onto the stack
void Interpreter::push(ObjectReference* value, instruction::PushType type) {
	if(type < 0) {
		this->stack[this->stack.head].setObject(value);
		this->stack.pushed();
	}
	else {
		this->returnRegister.setObject(value);
	}
}

void Interpreter::pop() {
	Entry &test = this->stack[this->stack.head - 1];
	test.erase();
	this->stack.popped();
}

void Interpreter::startInterpretation(Instruction* head) {
	InstructionContainer* container = new InstructionContainer(head);
	this->pushFunctionFrame(container); // create the instructions
	this->startTime = getMicrosecondsNow();
	this->interpret();
	delete container;
}

void Interpreter::execFile(string filename) {
	if(this->isParallel) {
		this->execFilenames.push(filename);
		return;
	}
	this->actuallyExecFile(filename);
}

void Interpreter::actuallyExecFile(string filename) {
	// ParsedArguments args;
	// Tokenizer tokenizer(filename, args);
	// Parser parser(&tokenizer, args);
	
	// this->pushFunctionFrame(new InstructionContainer(ts::Compile(&parser, this)));
	// this->interpret();
}

Entry* Interpreter::handleTSSLParent(string &name, unsigned int argc, Entry* argv, entry::EntryType* argumentTypes) {
	FunctionFrame &frame = this->frames[this->frames.head - 1];
	MethodTreeEntry* methodTreeEntry = frame.methodTreeEntry;
	int methodTreeEntryIndex = frame.methodTreeEntryIndex + 1; // always go up in the method tree
	PackagedFunctionList* list;
	int packagedFunctionListIndex;

	if((size_t)methodTreeEntryIndex < methodTreeEntry->list.head) {
		list = methodTreeEntry->list[methodTreeEntryIndex];
		packagedFunctionListIndex = list->topValidIndex;

		Function* foundFunction = (*list)[packagedFunctionListIndex];
		if(foundFunction->isTSSL) {
			sl::Function* function = foundFunction->function;
			this->pushTSSLFunctionFrame(methodTreeEntry, methodTreeEntryIndex);
			Entry* returnValue = function->function(this->engine, argc, argv);
			this->popFunctionFrame();
			return returnValue;
		}
		else {
			// push arguments onto the stack
			for(size_t i = 0; i < argc; i++) {
				this->push(argv[i], instruction::STACK);
			}

			this->push((double)argc, instruction::STACK);
			
			this->pushFunctionFrame(
				foundFunction,
				list,
				packagedFunctionListIndex,
				methodTreeEntry,
				methodTreeEntryIndex,
				argc + 1,
				foundFunction->variableCount
			);
			this->interpret();

			return new Entry(this->returnRegister);
		}
	}

	return new Entry(getEmptyString());
}

void Interpreter::warning(const char* format, ...) {
	if(this->warnings) {
		va_list argptr;
		va_start(argptr, format);
		printWarning(format, argptr);
		va_end(argptr);
	}
}

bool Interpreter::tick() {
	start_tick:
	
	unsigned long long time = getMicrosecondsNow();

	Schedule* schedule = this->schedules.top();
	while(this->schedules.array.head > 0 && time > schedule->end) {
		if(schedule->object != nullptr) {
			this->callMethod(schedule->object, schedule->functionName, schedule->arguments, schedule->argumentCount);
		}
		else {
			this->callFunction(schedule->functionName, schedule->arguments, schedule->argumentCount);
		}

		this->schedules.pop();
		schedule = this->schedules.top();
	}

	// process queued files for parallel mode
	if(this->isParallel) {
		while(this->engine->fileQueue.size() != 0) {
			string filename = this->engine->fileQueue.front();
			this->engine->fileQueue.pop();

			this->engine->execFile(filename, true);
		}

		while(this->engine->shellQueue.size() != 0) {
			string shell = this->engine->shellQueue.front();
			this->engine->shellQueue.pop();

			this->engine->execShell(shell, true);
		}

		this_thread::sleep_for(chrono::milliseconds(this->tickRate));
		goto start_tick;
	}

	// return false if we have schedules left, return true if there's none left
	return this->schedules.array.head == 0;
}

void Interpreter::setTickRate(long tickRate) {
	this->tickRate = tickRate;
}

void Interpreter::interpret() {
	start:
	Instruction &instruction = this->topContainer->array[*this->instructionPointer];
	(*this->instructionPointer)++;

	// PrintInstruction(instruction);
	
	switch(instruction.type) {
		case instruction::INVALID_INSTRUCTION: {
			this->popFunctionFrame();

			if(this->showTime && this->frames.head == 0) {
				(*this->engine->printFunction)("exec time: %lld\n", getMicrosecondsNow() - this->startTime);
			}
			// this->stack.head = 0;

			return;	
		}

		// generate code for math instructions
		## math_generator.py

		// generate code for assignment instructions
		## assignments_generator.py
		
		case instruction::NOOP: {
			break;
		}

		case instruction::PUSH: { // push to the stack
			this->push(instruction.push.entry, instruction.pushType);
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
			int number = 0;
			## type_conversion.py entry number NUMBER_STRING_OBJECT NUMBER

			if(number != 0) {
				*this->instructionPointer = instruction.jumpIfTrue.index;
			}

			if(instruction.jumpIfTrue.pop) {
				this->pop();
			}
			break;
		}

		case instruction::JUMP_IF_FALSE: { // jump to an instruction
			Entry &entry = this->stack[this->stack.head - 1];
			int number = 1;
			## type_conversion.py entry number NUMBER_STRING_OBJECT NUMBER

			if(number == 0) {
				*this->instructionPointer = instruction.jumpIfFalse.index;
			}

			if(instruction.jumpIfFalse.pop) {
				this->pop();
			}
			break;
		}

		case instruction::UNARY_MATHEMATICS: {
			Entry* value;
			if(instruction.unaryMathematics.stackIndex < 0) {
				value = &this->stack[this->stack.head - 1];
			}
			else {
				value = &this->stack[instruction.unaryMathematics.stackIndex + this->stackFramePointer];
			}

			double valueNumber = 0;

			## type_conversion.py *value valueNumber NUMBER_STRING_OBJECT NUMBER
			
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
			this->push(result, instruction.pushType);
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

				this->push(entry, instruction.pushType);
			}
			else {
				this->push(this->stack[instruction.localAccess.stackIndex + this->stackFramePointer], instruction.pushType);
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

			this->push(entry, instruction.pushType);

			break;
		}

		case instruction::OBJECT_ACCESS: { // push object property to stack
			Entry &objectEntry = this->stack[this->stack.head - 1 - instruction.localAssign.dimensions];
			ObjectWrapper* objectWrapper = nullptr;

			## type_conversion.py objectEntry objectWrapper OBJECT_NUMBER_STRING OBJECT

			// if the object is not alive anymore, push nothing to the stack
			if(objectWrapper == nullptr) {
				this->pop(); // pop the object
				this->push(this->emptyEntry, instruction.pushType);
				break;
			}
			
			Entry &entry = objectWrapper->object->properties.getVariableEntry(
				instruction,
				instruction.localAccess.source,
				instruction.localAssign.hash
			);

			for(int i = 0; i < instruction.localAccess.dimensions; i++) {
				this->pop(); // pop the dimensions if we have any
			}

			this->pop(); // pop the object

			this->push(entry, instruction.pushType);

			break;
		}

		case instruction::SYMBOL_ACCESS: { // lookup object by name and push it to stack if it exists
			// try to look up the object's name
			auto objectIterator = this->stringToObject.find(instruction.symbolAccess.source, instruction.symbolAccess.hash);
			if(objectIterator == this->stringToObject.end()) {
				this->push(emptyEntry, instruction.pushType);
			}
			else {
				this->push(new ObjectReference(objectIterator->second), instruction.pushType);
			}
			
			break;
		}

		case instruction::CALL_FUNCTION: { // jump to a new instruction container
			if(!instruction.callFunction.isCached) {
				bool found = false;
				if(
					instruction.callFunction.nameSpace.length() != 0
					&& this->engine->namespaceToMethodTreeIndex.find(toLower(instruction.callFunction.nameSpace)) != this->engine->namespaceToMethodTreeIndex.end()
				) {
					size_t namespaceIndex = this->engine->namespaceToMethodTreeIndex[toLower(instruction.callFunction.nameSpace)];
					auto methodIndex = this->engine->methodNameToIndex.find(toLower(instruction.callFunction.name));

					if(methodIndex != this->engine->methodNameToIndex.end()) {
						auto methodEntry = this->engine->methodTrees[namespaceIndex]->methodIndexToEntry.find(methodIndex->second);
						if(methodEntry != this->engine->methodTrees[namespaceIndex]->methodIndexToEntry.end()) {
							instruction.callFunction.cachedEntry = methodEntry->second;
							instruction.callFunction.isCached = true;
							found = true;
						}
					}
				}
				else { // find non-namespace function
					if(this->engine->nameToFunctionIndex.find(toLower(instruction.callFunction.name)) != this->engine->nameToFunctionIndex.end()) {
						instruction.callFunction.cachedFunctionList
							= this->engine->functions[this->engine->nameToFunctionIndex[toLower(instruction.callFunction.name)]];

						instruction.callFunction.isCached = true;
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

					this->push(this->emptyEntry, instruction.pushType);
					break;
				}
			}

			Function* foundFunction;
			PackagedFunctionList* list;
			int packagedFunctionListIndex = -1;
			MethodTreeEntry* methodTreeEntry = nullptr;
			int methodTreeEntryIndex = -1;
			if(instruction.callFunction.cachedEntry != nullptr) {
				list = instruction.callFunction.cachedEntry->list[0];
				packagedFunctionListIndex = list->topValidIndex;
				foundFunction = (*list)[packagedFunctionListIndex];
			}
			else {
				list = instruction.callFunction.cachedFunctionList;
				packagedFunctionListIndex = list->topValidIndex;
				foundFunction = (*list)[packagedFunctionListIndex];
			}

			## call_generator.py

			break;
		}

		case instruction::RETURN: { // return from a function
			this->popFunctionFrame();

			// if the current function frame is TSSL, then we're in a C++ PARENT(...) operation and we need to quit
			// here so the original TSSL method can take over
			if(this->frames[this->frames.head - 1].isTSSL) {
				return;
			}

			// if we just ran out of instruction containers, just die here
			if(this->topContainer == nullptr) {
				return;
			}

			if(instruction.functionReturn.hasValue) {
				this->push(this->returnRegister, instruction::STACK, true); // push return register
			}
			else {
				this->push(this->emptyEntry, instruction::STACK);
			}
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
				this->push(getEmptyString(), instruction.pushType);
			}

			break;
		}

		case instruction::CREATE_OBJECT: {
			string typeName = instruction.createObject.typeName;
			string symbolName = instruction.createObject.symbolName;
			string classProperty = instruction.createObject.classProperty;
			string superClassProperty = instruction.createObject.superClassProperty;
			if(!instruction.createObject.isCached) {
				// handle super class property stuff
				if(!instruction.createObject.superClassPropertyCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* superClassPropertyCStr;
					## type_conversion.py entry superClassPropertyCStr OBJECT_NUMBER_STRING STRING
					superClassProperty = string(superClassPropertyCStr);
					this->pop();
				}

				// handle class property stuff
				if(!instruction.createObject.classPropertyCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* classPropertyCStr;
					## type_conversion.py entry classPropertyCStr OBJECT_NUMBER_STRING STRING
					classProperty = string(classPropertyCStr);
					this->pop();
				}
				
				// handle symbol name stuff
				if(!instruction.createObject.symbolNameCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* symbolNameCStr;
					## type_conversion.py entry symbolNameCStr OBJECT_NUMBER_STRING STRING
					symbolName = string(symbolNameCStr);
					this->pop();
				}

				// handle type name stuff
				if(!instruction.createObject.typeNameCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* typeNameCStr;
					## type_conversion.py entry typeNameCStr OBJECT_NUMBER_STRING STRING
					typeName = string(typeNameCStr);
					this->pop();
				}

				// check to make sure that the type name that we're using is defined by the TSSL. if not, we can't
				// create the object
				MethodTree* typeCheck = this->engine->getNamespace(typeName);
				if(typeCheck == nullptr || !typeCheck->isTSSL) {
					this->warning("could not create object with type '%s'\n", typeName.c_str());
					this->push(getEmptyString(), instruction.pushType);
					break;
				}

				instruction.createObject.typeMethodTree = typeCheck;

				MethodTree* tree = this->engine->createMethodTreeFromNamespaces(
					symbolName,
					classProperty,
					superClassProperty,
					typeName
				);

				instruction.createObject.methodTree = tree;
			}
			else if(!instruction.createObject.canCreate) {
				this->warning("could not create object with type '%s'\n", instruction.createObject.typeName.c_str());
				this->push(getEmptyString(), instruction.pushType);
				break;
			}
			
			ObjectWrapper* object = ts::CreateObject(
				this,
				typeName,
				instruction.createObject.inheritedName,
				instruction.createObject.methodTree,
				instruction.createObject.typeMethodTree
			);

			if(symbolName.length() != 0) {
				this->setObjectName(symbolName, object);
			}

			this->push(new ObjectReference(object), instruction.pushType);
			break;
		}

		case instruction::CALL_OBJECT: {
			Entry &numberOfArguments = this->stack[this->stack.head - 1];
			int argumentCount = (int)numberOfArguments.numberData;
			
			// pull the object from the stack
			Entry &objectEntry = this->stack[this->stack.head - 1 - argumentCount];
			ObjectWrapper* objectWrapper = nullptr;
			Object* object = nullptr;
			## type_conversion.py objectEntry objectWrapper OBJECT_NUMBER_STRING OBJECT

			if(objectWrapper == nullptr) {
				// pop arguments that we didn't use
				Entry &numberOfArguments = this->stack[this->stack.head - 1];
				int number = (int)numberOfArguments.numberData;
				for(int i = 0; i < number + 1; i++) {
					this->pop();
				}

				this->push(this->emptyEntry, instruction.pushType);
				break;
			}

			object = objectWrapper->object;

			// cache the method entry pointer in the instruction
			if(instruction.callObject.isCached == false) {
				bool found = false;
				auto methodNameIndex = this->engine->methodNameToIndex.find(toLower(instruction.callObject.name));
				if(methodNameIndex != this->engine->methodNameToIndex.end()) {
					auto methodEntry = object->methodTree->methodIndexToEntry.find(methodNameIndex->second);
					if(methodEntry != object->methodTree->methodIndexToEntry.end()) {
						instruction.callObject.cachedEntry = methodEntry->second;
						instruction.callObject.isCached = true;
						found = true;
					}
				}
				
				if(!found) {
					this->warning("could not find function with name '%s::%s'\n", object->nameSpace.c_str(), instruction.callFunction.name.c_str());

					// pop arguments that we didn't use
					Entry &numberOfArguments = this->stack[this->stack.head - 1];
					int number = (int)numberOfArguments.numberData;
					for(int i = 0; i < number + 1; i++) {
						this->pop();
					}

					this->push(this->emptyEntry, instruction.pushType);
					break;
				}
			}

			// look up the method in the method tree
			MethodTreeEntry* methodTreeEntry = instruction.callObject.cachedEntry;
			int methodTreeEntryIndex = instruction.callObject.cachedEntry->hasInitialMethod ? 0 : 1;
			PackagedFunctionList* list = methodTreeEntry->list[methodTreeEntryIndex];
			size_t packagedFunctionListIndex = list->topValidIndex;
			Function* foundFunction = (*list)[packagedFunctionListIndex];
			## call_generator.py
			
			break;
		}

		case instruction::LINK_VARIABLE: {
			this->topContext->linkVariable(instruction.linkVariable.source, instruction.linkVariable.hash, instruction.linkVariable.stackIndex);
			break;
		}

		case instruction::CALL_PARENT: {
			FunctionFrame &frame = this->frames[this->frames.head - 1];
			MethodTreeEntry* methodTreeEntry = frame.methodTreeEntry;
			int methodTreeEntryIndex = frame.methodTreeEntryIndex;
			PackagedFunctionList* list = frame.packagedFunctionList;
			int packagedFunctionListIndex = frame.packagedFunctionList->getNextValidIndex(frame.packagedFunctionListIndex);

			if(packagedFunctionListIndex == -1 && methodTreeEntry != nullptr) { // walk the method tree
				methodTreeEntryIndex++;
				if((size_t)methodTreeEntryIndex < methodTreeEntry->list.head) {
					list = methodTreeEntry->list[methodTreeEntryIndex];
					packagedFunctionListIndex = list->topValidIndex;

					Function* foundFunction = (*list)[packagedFunctionListIndex];
					## call_generator.py
				}
				else {
					// pop arguments that we didn't use
					Entry &numberOfArguments = this->stack[this->stack.head - 1];
					int number = (int)numberOfArguments.numberData;
					for(int i = 0; i < number + 1; i++) {
						this->pop();
					}

					this->push(this->emptyEntry, instruction.pushType);
					break;
				}
			}
			else if(packagedFunctionListIndex != -1) {
				Function* foundFunction = (*list)[packagedFunctionListIndex];
				## call_generator.py
			}
			else {
				// pop arguments that we didn't use
				Entry &numberOfArguments = this->stack[this->stack.head - 1];
				int number = (int)numberOfArguments.numberData;
				for(int i = 0; i < number + 1; i++) {
					this->pop();
				}

				this->push(this->emptyEntry, instruction.pushType);
				break;
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

void Interpreter::addSchedule(unsigned long long time, string functionName, Entry* arguments, size_t argumentCount, ObjectReference* object) {
	this->schedules.insert(new Schedule(
		time,
		getMicrosecondsNow(),
		functionName,
		arguments,
		argumentCount,
		object
	));
}

void Interpreter::setObjectName(string &name, ObjectWrapper* object) {
	this->stringToObject[name] = object;
	object->object->setName(name);
}

void Interpreter::deleteObjectName(string &name) {
	this->stringToObject.erase(name);
}

Entry* Interpreter::callFunction(string functionName, Entry* arguments, size_t argumentCount) {
	// set up function call frame
	Function* foundFunction;
	PackagedFunctionList* list;
	int packagedFunctionListIndex = -1;
	MethodTreeEntry* methodTreeEntry = nullptr;
	int methodTreeEntryIndex = -1;

	if(this->engine->nameToFunctionIndex.find(toLower(functionName)) != this->engine->nameToFunctionIndex.end()) {
		list = this->engine->functions[this->engine->nameToFunctionIndex[toLower(functionName)]];
		packagedFunctionListIndex = list->topValidIndex;
		foundFunction = (*list)[packagedFunctionListIndex];
	}
	else {
		this->warning("could not find function with name '%s'\n", functionName.c_str());
		return new Entry(getEmptyString());
	}

	if(foundFunction->isTSSL) { // TODO handle argument type conversion
		sl::Function* function = foundFunction->function;
		this->pushTSSLFunctionFrame(methodTreeEntry, methodTreeEntryIndex);
		Entry* result = function->function(this->engine, argumentCount, arguments);
		this->popFunctionFrame();

		if(result == nullptr) {
			return new Entry(getEmptyString());
		}
		return result;
	}
	else {
		// push arguments onto the stack
		for(size_t i = 0; i < argumentCount; i++) {
			this->push(arguments[i], instruction::STACK);
		}

		this->push((double)argumentCount, instruction::STACK);

		// handle callback
		this->pushFunctionFrame(
			foundFunction,
			list,
			packagedFunctionListIndex,
			methodTreeEntry,
			methodTreeEntryIndex,
			argumentCount + 1,
			foundFunction->variableCount
		);
		this->interpret();
		return new Entry(this->returnRegister);
	}
}

Entry* Interpreter::callMethod(ObjectReference* objectReference, string methodName, Entry* arguments, size_t argumentCount) {
	// set up function call frame
	Function* foundFunction;
	PackagedFunctionList* list;
	int packagedFunctionListIndex = -1;
	MethodTreeEntry* methodTreeEntry = nullptr;
	int methodTreeEntryIndex = -1;

	ObjectWrapper* objectWrapper = objectReference->objectWrapper;
	Object* object = nullptr;
	if(objectWrapper == nullptr) {
		return new Entry(getEmptyString());
	}

	object = objectWrapper->object;
	
	bool found = false;
	auto methodNameIndex = this->engine->methodNameToIndex.find(toLower(methodName));
	if(methodNameIndex != this->engine->methodNameToIndex.end()) {
		auto methodEntry = object->methodTree->methodIndexToEntry.find(methodNameIndex->second);
		if(methodEntry != object->methodTree->methodIndexToEntry.end()) {
			methodTreeEntry = methodEntry->second;
			methodTreeEntryIndex = methodTreeEntry->hasInitialMethod ? 0 : 1;
			list = methodTreeEntry->list[methodTreeEntryIndex];
			packagedFunctionListIndex = list->topValidIndex;
			foundFunction = (*list)[packagedFunctionListIndex];
			found = true;
		}
	}

	if(!found) {
		this->warning("could not find function with name '%s::%s'\n", object->nameSpace.c_str(), methodName.c_str());
		return new Entry(getEmptyString());
	}

	if(foundFunction->isTSSL) { // TODO handle argument type conversion
		sl::Function* function = foundFunction->function;
		this->pushTSSLFunctionFrame(methodTreeEntry, methodTreeEntryIndex);
		Entry* output = function->function(this->engine, argumentCount, arguments);
		this->popFunctionFrame();

		if(output == nullptr) {
			return new Entry(getEmptyString());
		}
		return output;
	}
	else {
		// push arguments onto the stack
		for(size_t i = 0; i < argumentCount; i++) {
			this->push(arguments[i], instruction::STACK);
		}

		this->push((double)argumentCount, instruction::STACK);

		// handle callback
		this->pushFunctionFrame(
			foundFunction,
			list,
			packagedFunctionListIndex,
			methodTreeEntry,
			methodTreeEntryIndex,
			argumentCount + 1,
			foundFunction->variableCount
		);
		this->interpret();

		return new Entry(this->returnRegister);
	}
}