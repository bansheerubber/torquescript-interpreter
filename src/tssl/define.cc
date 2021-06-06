#include "define.h"

#include "echo.h"
#include "getWord.h"

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
	type* s = new type[TS_ARG_COUNT] { type::STRING };
	type* sn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER };
	type* snn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::NUMBER };
	type* sns = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::STRING };

	FUNC_DEF(type::VOID, &echo, "echo", 1, s);

	FUNC_DEF(type::STRING, &firstWord, "firstWord", 1, s);
	FUNC_DEF(type::STRING, &restWords, "restWords", 1, s);
	FUNC_DEF(type::STRING, &getWord, "getWord", 2, sn);
	FUNC_DEF(type::STRING, &getWords, "getWords", 3, snn);
	FUNC_DEF(type::NUMBER, &getWordCount, "getWordCount", 1, s);
	FUNC_DEF(type::STRING, &removeWord, "removeWord", 2, sn);
	FUNC_DEF(type::STRING, &setWord, "setWord", 3, sns);
}