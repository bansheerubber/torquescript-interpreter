#include "interpreter.h"

#include "../util/cloneString.h"
#include "../compiler/compiler.h"
#include "debug.h"
#include "../tssl/define.h"
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

void ts::initMethodTree(Interpreter* interpreter, MethodTree** tree) {
	*tree = nullptr;
}

void ts::initSchedule(Interpreter* interpreter, Schedule** schedule) {
	*schedule = nullptr;
}

Interpreter::Interpreter(ParsedArguments args, bool isParallel) {
	this->isParallel = isParallel;
	
	this->emptyEntry.setString(getEmptyString());

	ts::sl::define(this);

	if(args.arguments["no-warnings"] != "") {
		this->warnings = false;
	}

	if(args.arguments["time"] != "") {
		this->showTime = true;
	}

	this->globalContext = VariableContext(this);

	if(this->isParallel) {
		this->tickThread = thread(&Interpreter::tick, this);
	}
}

Interpreter::~Interpreter() {
	for(size_t i = 0; i < this->functions.head; i++) {
		delete this->functions[i];
	}

	for(size_t i = 0; i < this->methodTrees.head; i++) {
		delete this->methodTrees[i];
	}
}

void Interpreter::defineTSSLMethodTree(MethodTree* tree) {
	string nameSpace = tree->name;
	if(this->namespaceToMethodTreeIndex.find(toLower(nameSpace)) == this->namespaceToMethodTreeIndex.end()) {
		this->namespaceToMethodTreeIndex[toLower(nameSpace)] = this->methodTrees.head;
		tree->index = this->methodTrees.head;
		this->methodTrees[this->methodTrees.head] = tree;
		this->methodTrees.pushed();
	}
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
		list->defineInitialFunction(container);
	}
	else {
		MethodTree* tree = this->methodTrees[this->namespaceToMethodTreeIndex[toLower(function->nameSpace)]];

		// associate the method name with an index
		size_t index = 0;
		if(this->methodNameToIndex.find(toLower(function->name)) == this->methodNameToIndex.end()) {
			this->methodNameToIndex[toLower(function->name)] = index = this->currentMethodNameIndex;
			this->currentMethodNameIndex++;
		}
		else {
			index = this->methodNameToIndex[toLower(function->name)];
		}

		tree->defineInitialMethod(function->name, index, container);
	}
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
	this->topContext = &frame.context;
	
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
		this->topContext = &frame.context;

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

	if(test.type == entry::OBJECT && test.objectData) {
		delete test.objectData;
		test.objectData = nullptr;
	}
	
	this->stack.popped();
}

void Interpreter::startInterpretation(Instruction* head) {
	this->pushFunctionFrame(new InstructionContainer(head)); // create the instructions
	this->startTime = getMicrosecondsNow();
	this->interpret();
}

void Interpreter::execFile(string filename) {
	if(this->isParallel) {
		this->execFilenames.push(filename);
		return;
	}
	this->actuallyExecFile(filename);
}

void Interpreter::actuallyExecFile(string filename) {
	ParsedArguments args;
	Tokenizer tokenizer(filename, args);
	Parser parser(&tokenizer, args);
	
	this->pushFunctionFrame(new InstructionContainer(ts::Compile(&parser, this)));
	this->interpret();
}

Entry* Interpreter::handleTSSLParent(string &name, size_t argc, Entry* argv, entry::EntryType* argumentTypes) {
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
			Entry* returnValue = function->function(this, argc, argv);
			this->popFunctionFrame();
			return returnValue;
		}
		else {
			// push arguments onto the stack
			for(size_t i = 0; i < argc; i++) {
				this->push(argv[i]);
			}

			this->push((double)argc);
			
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

void Interpreter::tick() {
	unsigned long long time = getMicrosecondsNow();

	Schedule* schedule = this->schedules.top();
	while(this->schedules.array.head > 0 && time > schedule->end) {
		// set up function call frame
		Function* foundFunction;
		PackagedFunctionList* list;
		int packagedFunctionListIndex = -1;
		MethodTreeEntry* methodTreeEntry = nullptr;
		int methodTreeEntryIndex = -1;

		if(schedule->object != nullptr) {
			Object* object = schedule->object->object;

			if(object == nullptr) {
				continue;
			}
			
			bool found = false;
			auto methodNameIndex = this->methodNameToIndex.find(toLower(schedule->functionName));
			if(methodNameIndex != this->methodNameToIndex.end()) {
				auto methodEntry = this->methodTrees[object->namespaceIndex]->methodIndexToEntry.find(methodNameIndex->second);
				if(methodEntry != this->methodTrees[object->namespaceIndex]->methodIndexToEntry.end()) {
					methodTreeEntry = methodEntry->second;
					methodTreeEntryIndex = methodTreeEntry->hasInitialMethod ? 0 : 1;
					list = methodTreeEntry->list[methodTreeEntryIndex];
					packagedFunctionListIndex = list->topValidIndex;
					foundFunction = (*list)[packagedFunctionListIndex];
					found = true;
				}
			}

			if(!found) {
				continue;
			}
		}
		else {
			if(this->nameToFunctionIndex.find(toLower(schedule->functionName)) != this->nameToFunctionIndex.end()) {
				list = this->functions[this->nameToFunctionIndex[toLower(schedule->functionName)]];
				packagedFunctionListIndex = list->topValidIndex;
				foundFunction = (*list)[packagedFunctionListIndex];
			}
			else {
				continue;
			}
		}

		if(foundFunction->isTSSL) {
			sl::Function* function = foundFunction->function;
			this->pushTSSLFunctionFrame(methodTreeEntry, methodTreeEntryIndex);
			function->function(this, schedule->argumentCount, schedule->arguments);
			this->popFunctionFrame();
		}
		else {
			// push arguments onto the stack
			for(size_t i = 0; i < schedule->argumentCount; i++) {
				this->push(schedule->arguments[i]);
			}

			this->push((double)schedule->argumentCount);

			// handle callback
			this->pushFunctionFrame(
				foundFunction,
				list,
				packagedFunctionListIndex,
				methodTreeEntry,
				methodTreeEntryIndex,
				schedule->argumentCount + 1,
				foundFunction->variableCount
			);
			this->interpret();
		}

		this->schedules.pop();
		schedule = this->schedules.top();
	}

	// process queued files for parallel mode
	if(this->isParallel) {
		while(this->execFilenames.size() != 0) {
			string filename = this->execFilenames.front();
			this->execFilenames.pop();

			this->actuallyExecFile(filename);
		}

		this_thread::sleep_for(chrono::milliseconds(this->tickRate));
		this->tick();
	}
}

void Interpreter::setTickRate(long tickRate) {
	this->tickRate = tickRate;
}

void Interpreter::interpret() {
	start:
	Instruction &instruction = this->topContainer->array[*this->instructionPointer];
	
	if(*this->instructionPointer >= this->topContainer->size) { // quit once we run out of instructions
		this->popFunctionFrame();

		if(this->showTime) {
			printf("%lld\n", getMicrosecondsNow() - this->startTime);
		}
		this->stack.head = 0;

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
			Entry &value = this->stack[this->stack.head - 1];
			double valueNumber = 0;

			## type_conversion.py value valueNumber NUMBER_STRING_OBJECT NUMBER
			
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
			Object* object = nullptr;

			## type_conversion.py objectEntry object OBJECT_NUMBER_STRING OBJECT

			// if the object is not alive anymore, push nothing to the stack
			if(object == nullptr) {
				this->pop(); // pop the object
				this->push(this->emptyEntry);
				break;
			}
			
			Entry &entry = object->properties.getVariableEntry(
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

		case instruction::SYMBOL_ACCESS: { // lookup object by name and push it to stack if it exists
			// try to look up the object's name
			auto objectIterator = this->stringToObject.find(instruction.symbolAccess.source, instruction.symbolAccess.hash);
			if(objectIterator == this->stringToObject.end()) {
				this->push(emptyEntry);
			}
			else {
				this->push(new ObjectReference(objectIterator->second));
			}
			
			break;
		}

		case instruction::CALL_FUNCTION: { // jump to a new instruction container
			if(!instruction.callFunction.isCached) {
				bool found = false;
				if(
					instruction.callFunction.nameSpace.length() != 0
					&& this->namespaceToMethodTreeIndex.find(toLower(instruction.callFunction.nameSpace)) != this->namespaceToMethodTreeIndex.end()
				) {
					size_t namespaceIndex = this->namespaceToMethodTreeIndex[toLower(instruction.callFunction.nameSpace)];
					auto methodIndex = this->methodNameToIndex.find(toLower(instruction.callFunction.name));

					if(methodIndex != this->methodNameToIndex.end()) {
						auto methodEntry = this->methodTrees[namespaceIndex]->methodIndexToEntry.find(methodIndex->second);
						if(methodEntry != this->methodTrees[namespaceIndex]->methodIndexToEntry.end()) {
							instruction.callFunction.cachedEntry = methodEntry->second;
							instruction.callFunction.isCached = true;
							found = true;
						}
					}
				}
				else { // find non-namespace function
					if(this->nameToFunctionIndex.find(toLower(instruction.callFunction.name)) != this->nameToFunctionIndex.end()) {
						instruction.callFunction.cachedFunctionList = this->functions[this->nameToFunctionIndex[toLower(instruction.callFunction.name)]];
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

					this->push(this->emptyEntry);
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
			if(instruction.functionReturn.hasValue) {
				copyEntry(this->stack[this->stack.head - 1], this->returnRegister);
				this->pop(); // pop return value
			}
			else {
				copyEntry(this->emptyEntry, this->returnRegister);
			}

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
			string typeName = instruction.createObject.typeName;
			string symbolName = instruction.createObject.symbolName;
			if(!instruction.createObject.isCached) {
				// handle symbol name stuff
				if(!instruction.createObject.symbolNameCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* symbolNameCStr;
					## type_conversion.py entry symbolNameCStr OBJECT_NUMBER_STRING STRING
					symbolName = string(symbolNameCStr);
					this->pop();
				}
				else {
					symbolName = instruction.createObject.symbolName;
				}

				// handle type name stuff
				if(!instruction.createObject.typeNameCached) {
					Entry &entry = this->stack[this->stack.head - 1];
					char* typeNameCStr;
					## type_conversion.py entry typeNameCStr OBJECT_NUMBER_STRING STRING
					typeName = string(typeNameCStr);
					this->pop();
				}
				else {
					typeName = instruction.createObject.typeName;
				}

				// check to make sure that the type name that we're using is defined by the TSSL. if not, we can't
				// create the object
				MethodTree* typeCheck = this->getNamespace(typeName);
				if(typeCheck == nullptr || !typeCheck->isTSSL) {
					this->warning("could not create object with type '%s'\n", typeName.c_str());
					this->push(getEmptyString());
					break;
				}

				MethodTree* tree = this->createMethodTreeFromNamespaces(
					symbolName,
					typeName
				);

				instruction.createObject.methodTreeIndex = tree->index;
			}
			else if(!instruction.createObject.canCreate) {
				this->warning("could not create object with type '%s'\n", instruction.createObject.typeName.c_str());
				this->push(getEmptyString());
				break;
			}
			
			Object* object = new Object(
				this,
				typeName,
				instruction.createObject.inheritedName,
				instruction.createObject.methodTreeIndex
			);

			if(symbolName.length() != 0) {
				this->setObjectName(symbolName, object);
			}

			this->push(new ObjectReference(object));
			break;
		}

		case instruction::CALL_OBJECT: {
			Entry &numberOfArguments = this->stack[this->stack.head - 1];
			int argumentCount = (int)numberOfArguments.numberData;
			
			// pull the object from the stack
			Entry &objectEntry = this->stack[this->stack.head - 1 - argumentCount];
			Object* object = nullptr;
			## type_conversion.py objectEntry object OBJECT_NUMBER_STRING OBJECT

			if(object == nullptr) {
				// pop arguments that we didn't use
				Entry &numberOfArguments = this->stack[this->stack.head - 1];
				int number = (int)numberOfArguments.numberData;
				for(int i = 0; i < number + 1; i++) {
					this->pop();
				}

				this->push(this->emptyEntry);
				break;
			}

			// cache the method entry pointer in the instruction
			if(instruction.callObject.isCached == false) {
				bool found = false;
				auto methodNameIndex = this->methodNameToIndex.find(toLower(instruction.callObject.name));
				if(methodNameIndex != this->methodNameToIndex.end()) {
					auto methodEntry = this->methodTrees[object->namespaceIndex]->methodIndexToEntry.find(methodNameIndex->second);
					if(methodEntry != this->methodTrees[object->namespaceIndex]->methodIndexToEntry.end()) {
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

					this->push(this->emptyEntry);
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

					this->push(this->emptyEntry);
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

				this->push(this->emptyEntry);
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
	list->defineInitialFunction(container);
}

void Interpreter::defineMethod(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
	Function* container = new Function(output.first, argumentCount, variableCount, name, nameSpace);

	// define the method tree if we don't have one yet
	MethodTree* tree;
	if(this->namespaceToMethodTreeIndex.find(toLower(nameSpace)) == this->namespaceToMethodTreeIndex.end()) {
		this->namespaceToMethodTreeIndex[toLower(nameSpace)] = this->methodTrees.head;
		tree = new MethodTree(nameSpace, this->methodTrees.head);
		this->methodTrees[this->methodTrees.head] = tree;
		this->methodTrees.pushed();
	}
	else {
		tree = this->methodTrees[this->namespaceToMethodTreeIndex[toLower(nameSpace)]];
	}

	// associate the method name with an index
	size_t index = 0;
	if(this->methodNameToIndex.find(toLower(name)) == this->methodNameToIndex.end()) {
		this->methodNameToIndex[toLower(name)] = index = this->currentMethodNameIndex;
		this->currentMethodNameIndex++;
	}
	else {
		index = this->methodNameToIndex[toLower(name)];
	}

	tree->defineInitialMethod(name, index, container);
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

void Interpreter::addPackageMethod(
	Package* package,
	string &nameSpace,
	string &name,
	InstructionReturn output,
	size_t argumentCount,
	size_t variableCount
) {
	// create the function container which we will use to execute the function at runtime
	Function* container = new Function(output.first, argumentCount, variableCount, name, nameSpace);
	
	// define the method tree if we don't have one yet
	MethodTree* tree;
	if(this->namespaceToMethodTreeIndex.find(toLower(nameSpace)) == this->namespaceToMethodTreeIndex.end()) {
		this->namespaceToMethodTreeIndex[toLower(nameSpace)] = this->methodTrees.head;
		tree = new MethodTree(nameSpace, this->methodTrees.head);
		this->methodTrees[this->methodTrees.head] = tree;
		this->methodTrees.pushed();
	}
	else {
		tree = this->methodTrees[this->namespaceToMethodTreeIndex[toLower(nameSpace)]];
	}

	// associate the method name with an index
	size_t index = 0;
	if(this->methodNameToIndex.find(toLower(name)) == this->methodNameToIndex.end()) {
		this->methodNameToIndex[toLower(name)] = index = this->currentMethodNameIndex;
		this->currentMethodNameIndex++;
	}
	else {
		index = this->methodNameToIndex[toLower(name)];
	}

	tree->addPackageMethod(name, index, container);
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

MethodTree* Interpreter::createMethodTreeFromNamespace(string nameSpace) {
	MethodTree* tree;
	auto iterator = this->namespaceToMethodTreeIndex.find(toLower(nameSpace));
	if(iterator == this->namespaceToMethodTreeIndex.end()) {
		this->namespaceToMethodTreeIndex[toLower(nameSpace)] = this->methodTrees.head;
		tree = new MethodTree(nameSpace, this->methodTrees.head);
		this->methodTrees[this->methodTrees.head] = tree;
		this->methodTrees.pushed();
	}
	else {
		tree = this->methodTrees[iterator->second];
	}

	return tree;
}

MethodTree* Interpreter::getNamespace(string nameSpace) {
	auto iterator = this->namespaceToMethodTreeIndex.find(toLower(nameSpace));
	if(iterator == this->namespaceToMethodTreeIndex.end()) {
		return nullptr;
	}
	else {
		return this->methodTrees[iterator->second];
	}
}

MethodTree* Interpreter::createMethodTreeFromNamespaces(
	string namespace1,
	string namespace2,
	string namespace3,
	string namespace4,
	string namespace5
) {
	string names[] = {
		namespace1,
		namespace2,
		namespace3,
		namespace4,
		namespace5,
	};
	
	string nameSpace = MethodTree::GetComplexNamespace(
		namespace1,
		namespace2,
		namespace3,
		namespace4,
		namespace5
	);

	MethodTree* tree = nullptr;
	auto iterator = this->namespaceToMethodTreeIndex.find(toLower(nameSpace));
	if(iterator == this->namespaceToMethodTreeIndex.end()) {
		tree = this->createMethodTreeFromNamespace(nameSpace);
		for(size_t i = 0; i < 5; i++) {
			if(names[i].length() != 0 && names[i] != nameSpace) {
				MethodTree* tree2 = this->createMethodTreeFromNamespace(names[i]);
				tree->addParent(tree2);
			}
		}
	}
	else {
		tree = this->methodTrees[iterator->second];
	}

	return tree;
}

void Interpreter::setObjectName(string &name, Object* object) {
	this->stringToObject[name] = object;
	object->setName(name);
}

void Interpreter::deleteObjectName(string &name) {
	this->stringToObject.erase(name);
}
