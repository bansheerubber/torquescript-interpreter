#include "define.h"

#include "echo.h"

using namespace ts::functions;

vector<Function*> ts::functions::functions;
unordered_map<string, unsigned long> ts::functions::nameToIndex;

void ts::functions::FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, int argumentCount, type* argumentTypes) {
	Function* function = new Function;
	function->returnType = returnType;
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;
	nameToIndex[string(name)] = functions.size();
	functions.push_back(function);
}

void ts::functions::define() {
	type* echoArguments = new type[TS_ARG_COUNT] { type::STRING };
	FUNC_DEF(type::VOID, &echo, "echo", 1, echoArguments);
}