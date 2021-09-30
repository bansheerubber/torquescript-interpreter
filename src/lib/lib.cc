#include "libSymbols.h"

#include <stdio.h>
#include <string>

#include "../args.h"
#include "../compiler/compiler.h"
#include "../tssl/define.h"
#include "../engine/engine.h"
#include "../interpreter/interpreter.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

tsEnginePtr tsCreateEngine(char isParallel) {
	ParsedArguments args = {};
	return new Engine(args, isParallel);
}

bool tsTick(tsEnginePtr engine) {
	// only tick if we're not in parallel mode
	if(!((ts::Engine*)engine)->interpreter->isParallel) {
		return ((ts::Engine*)engine)->interpreter->tick();
	}
	return false;
}

void tsSetTickRate(tsEnginePtr engine, long tickRate) {
	((ts::Engine*)engine)->interpreter->setTickRate(tickRate);
}

void tsExecFile(tsEnginePtr engine, const char* filename) {
	((ts::Engine*)engine)->execFile(string(filename));
}

void tsEval(tsEnginePtr engine, const char* shell) {
	((ts::Engine*)engine)->execShell(string(shell));
}

void tsSetPrintFunction(tsEnginePtr engine, tsPrintFunction(print), tsPrintFunction(warning), tsPrintFunction(error)) {
	((ts::Engine*)engine)->printFunction = print;
	((ts::Engine*)engine)->warningFunction = warning;
	((ts::Engine*)engine)->errorFunction = error;
}

void tsVSetPrintFunction(tsEnginePtr engine, tsVPrintFunction(print), tsVPrintFunction(warning), tsVPrintFunction(error)) {
	((ts::Engine*)engine)->vPrintFunction = print;
	((ts::Engine*)engine)->vWarningFunction = warning;
	((ts::Engine*)engine)->vErrorFunction = error;
}

void tsRegisterNamespace(tsEnginePtr engine, const char* nameSpace) {
	ts::MethodTree* methodTree = ((ts::Engine*)engine)->createMethodTreeFromNamespace(nameSpace);
	methodTree->isTSSL = true;
	((ts::Engine*)engine)->defineTSSLMethodTree(methodTree);
}

void tsNamespaceInherit(tsEnginePtr engine, const char* parent, const char* child) {
	ts::MethodTree* methodTree = ((ts::Engine*)engine)->getNamespace(child);
	methodTree->addParent(((ts::Engine*)engine)->getNamespace(parent));
}

tsObjectReferencePtr tsCreateObject(tsEnginePtr engine, const char* nameSpace, void* data) {
	ts::MethodTree* methodTree = ((ts::Engine*)engine)->getNamespace(nameSpace);
	return (tsObjectReferencePtr)new ts::ObjectReference(
		CreateObject(((ts::Engine*)engine)->interpreter, nameSpace, "", methodTree, methodTree, data)
	);
}

void tsDeleteObject(tsObjectReferencePtr objectReference) {
	// only delete the object if its already deleted
	if(((ObjectReference*)objectReference)->objectWrapper != nullptr) {
		delete ((ObjectReference*)objectReference)->objectWrapper->object;
	}
}

const char* tsGetNamespaceFromObject(tsObjectReferencePtr object) {
	return ((ts::ObjectWrapper*)object->objectWrapper)->object->typeMethodTree->name.c_str();
}

int tsCompareNamespaceToObject(tsObjectReferencePtr object, const char* nameSpace) {
	return string(((ts::ObjectWrapper*)object->objectWrapper)->object->typeMethodTree->name) == string(nameSpace);
}

void tsRegisterFunction(tsEnginePtr engine, tsEntryType returnType, tsFunctionPtr function, const char* name, unsigned int argumentCount, tsEntryType* argTypes) {
	((ts::Engine*)engine)->defineTSSLFunction(
		ts::sl::FUNC_DEF((ts::entry::EntryType)returnType, (ts_func)function, name, argumentCount, (ts::entry::EntryType*)argTypes)
	);
}

void tsRegisterMethod(tsEnginePtr engine, tsEntryType returnType, tsFunctionPtr function, const char* nameSpace, const char* name, unsigned int argumentCount, tsEntryType* argTypes) {
	((ts::Engine*)engine)->defineTSSLFunction(
		ts::sl::FUNC_DEF((ts::entry::EntryType)returnType, (ts_func)function, nameSpace, name, argumentCount, (ts::entry::EntryType*)argTypes)
	);
}

tsEntryPtr tsCallFunction(tsEnginePtr engine, const char* functionName, unsigned int argumentCount, tsEntryPtr arguments) {
	return (tsEntryPtr)((ts::Engine*)engine)->interpreter->callFunction(string(functionName), (ts::Entry*)arguments, argumentCount);
}

tsEntryPtr tsCallMethod(tsEnginePtr engine, tsObjectReferencePtr object, const char* functionName, unsigned int argumentCount, tsEntryPtr arguments) {
	return nullptr;	
}
