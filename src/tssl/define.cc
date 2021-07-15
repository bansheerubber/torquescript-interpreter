#include "define.h"

#include <math.h>

#include "echo.h"
#include "getWord.h"
#include "../interpreter/interpreter.h"
#include "math.h"
#include "../interpreter/methodTree.h"
#include "scriptObject.h"
#include "simObject.h"

using namespace ts::sl;

vector<ts::sl::Function*> ts::sl::functions;
unordered_map<string, size_t> ts::sl::nameToIndex;

vector<MethodTree*> ts::sl::methodTrees;
unordered_map<string, size_t> ts::sl::methodTreeNameToIndex;

ts::sl::Function* ts::sl::FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, size_t argumentCount, type* argumentTypes) {
	ts::sl::Function* function = new ts::sl::Function;
	function->returnType = returnType;
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;

	return function;
}

ts::sl::Function* ts::sl::FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, size_t argumentCount, type* argumentTypes) {
	ts::sl::Function* function = new ts::sl::Function;
	function->returnType = returnType;
	function->nameSpace = string(nameSpace);
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;
	
	return function;
}

MethodTree* ts::sl::NAMESPACE_DEF(const char* name) {
	string nameString(name);
	return new MethodTree(nameString);
}

void ts::sl::define(Interpreter* interpreter) {
	// define namespaces and their inheristance structure
	vector<MethodTree*> methodTrees;

	MethodTree* SimObject = NAMESPACE_DEF("SimObject");
	methodTrees.push_back(SimObject);

	MethodTree* ScriptObject = NAMESPACE_DEF("ScriptObject");
	methodTrees.push_back(ScriptObject);
	ScriptObject->addParent(SimObject);

	for(MethodTree* tree: methodTrees) {
		interpreter->defineTSSLMethodTree(tree);
	}
	
	// define functions/methods
	vector<ts::sl::Function*> functions;
	type* s = new type[TS_ARG_COUNT] { type::STRING };
	type* n = new type[TS_ARG_COUNT] { type::NUMBER };
	type* sn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER };
	type* nn = new type[TS_ARG_COUNT] { type::NUMBER, type::NUMBER };
	type* snn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::NUMBER };
	type* sns = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::STRING };
	type* os = new type[TS_ARG_COUNT] { type::OBJECT, type::STRING };

	functions.push_back(FUNC_DEF(type::VOID, &echo, "echo", 1, s));

	functions.push_back(FUNC_DEF(type::STRING, &firstWord, "firstWord", 1, s));
	functions.push_back(FUNC_DEF(type::STRING, &restWords, "restWords", 1, s));
	functions.push_back(FUNC_DEF(type::STRING, &getWord, "getWord", 2, sn));
	functions.push_back(FUNC_DEF(type::STRING, &getWords, "getWords", 3, snn));
	functions.push_back(FUNC_DEF(type::NUMBER, &getWordCount, "getWordCount", 1, s));
	functions.push_back(FUNC_DEF(type::STRING, &removeWord, "removeWord", 2, sn));
	functions.push_back(FUNC_DEF(type::STRING, &setWord, "setWord", 3, sns));

	functions.push_back(FUNC_DEF(type::NUMBER, &mAbs, "mAbs", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mACos, "mACos", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mASin, "mASin", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mATan, "mATan", 2, nn));
	functions.push_back(FUNC_DEF(type::NUMBER, &mCeil, "mCeil", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mFloor, "mFloor", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mCos, "mCos", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mSin, "mSin", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mTan, "mTan", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mPow, "mPow", 2, nn));
	functions.push_back(FUNC_DEF(type::NUMBER, &mSqrt, "mSqrt", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mDegToRad, "mDegToRad", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mRadToDeg, "mRadToDeg", 1, n));
	functions.push_back(FUNC_DEF(type::NUMBER, &mLog, "mLog", 1, n));
	functions.push_back(FUNC_DEF(type::STRING, &mFloatLength, "mFloatLength", 2, n));

	functions.push_back(FUNC_DEF(type::VOID, &SimObject__test, "SimObject", "test", 2, os));
	functions.push_back(FUNC_DEF(type::VOID, &ScriptObject__test, "ScriptObject", "test", 2, os));

	for(ts::sl::Function* function: functions) {
		interpreter->defineTSSLFunction(function);
	}
}

void ts::sl::PARENT(Interpreter* interpreter, const char* methodName, size_t argc, void** argv) {
	printf("%s %ld\n", methodName, argc);
}
