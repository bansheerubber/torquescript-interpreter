#include "engine.h"

#include "../compiler/compiler.h"
#include "../tssl/define.h"
#include "../interpreter/instruction.h"
#include "../components/sourceFile.h"

using namespace ts;

void ts::initPackagedFunctionList(Engine* engine, PackagedFunctionList** list) {
	*list = nullptr;
}

void ts::initMethodTree(Engine* engine, MethodTree** tree) {
	*tree = nullptr;
}

Engine::Engine(ParsedArguments args, bool isParallel) {
	ts::sl::define(this);
	
	this->args = args;
	this->tokenizer = new Tokenizer(this, args);
	this->parser = new Parser(this, args);
	this->interpreter = new Interpreter(this, args, isParallel);
}

Engine::~Engine() {
	delete this->tokenizer;
	delete this->parser;
	delete this->interpreter;

	for(size_t i = 0; i < this->functions.head; i++) {
		delete this->functions[i];
	}

	for(size_t i = 0; i < this->methodTrees.head; i++) {
		delete this->methodTrees[i];
	}
}

void Engine::execFile(string fileName, bool forceExecution) {
	if(!this->interpreter->isParallel || forceExecution) {
		this->tokenizer->tokenizeFile(fileName);
		this->parser->startParse();

		// compile
		InstructionReturn result = parser->getSourceFile()->compile(this, {
			loop: nullptr,
			scope: nullptr,
		});
		
		if(this->interpreter->startTime == 0) {
			this->interpreter->startInterpretation(result.first);
		}
		else {
			this->interpreter->pushFunctionFrame(new InstructionContainer(result.first));
			this->interpreter->interpret();
		}
	}
	else if(this->interpreter->isParallel) {
		this->fileQueue.push(fileName);
	}
}

void Engine::execPiped(string piped) {
	this->tokenizer->tokenizePiped(piped);
	this->parser->startParse();

	// compile
	InstructionReturn result = parser->getSourceFile()->compile(this, {
		loop: nullptr,
		scope: nullptr,
	});

	this->interpreter->startInterpretation(result.first);	
}

void Engine::defineTSSLMethodTree(MethodTree* tree) {
	string nameSpace = tree->name;
	if(this->namespaceToMethodTreeIndex.find(toLower(nameSpace)) == this->namespaceToMethodTreeIndex.end()) {
		this->namespaceToMethodTreeIndex[toLower(nameSpace)] = this->methodTrees.head;
		tree->index = this->methodTrees.head;
		this->methodTrees[this->methodTrees.head] = tree;
		this->methodTrees.pushed();
	}
}

void Engine::defineTSSLFunction(sl::Function* function) {
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

void Engine::defineFunction(string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
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

void Engine::defineMethod(string &nameSpace, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
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

void Engine::addPackageFunction(Package* package, string &name, InstructionReturn output, size_t argumentCount, size_t variableCount) {
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

void Engine::addPackageMethod(
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

MethodTree* Engine::createMethodTreeFromNamespace(string nameSpace) {
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

MethodTree* Engine::getNamespace(string nameSpace) {
	auto iterator = this->namespaceToMethodTreeIndex.find(toLower(nameSpace));
	if(iterator == this->namespaceToMethodTreeIndex.end()) {
		return nullptr;
	}
	else {
		return this->methodTrees[iterator->second];
	}
}

MethodTree* Engine::createMethodTreeFromNamespaces(
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
