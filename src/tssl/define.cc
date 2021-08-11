#include "define.h"

#include <math.h>

#include "echo.h"
#include "getWord.h"
#include "../interpreter/interpreter.h"
#include "isObject.h"
#include "math.h"
#include "../interpreter/methodTree.h"
#include "schedule.h"
#include "scriptObject.h"
#include "simObject.h"
#include "string.h"

using namespace ts::sl;

vector<ts::sl::Function*> ts::sl::functions;
unordered_map<string, size_t> ts::sl::nameToIndex;

vector<MethodTree*> ts::sl::methodTrees;
unordered_map<string, size_t> ts::sl::methodTreeNameToIndex;

ts::sl::Function* ts::sl::FUNC_DEF(entry::EntryType returnType, TS_FUNC(functionPointer), const char* name, size_t argumentCount, entry::EntryType* argumentTypes) {
	ts::sl::Function* function = new ts::sl::Function;
	function->returnType = returnType;
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;

	return function;
}

ts::sl::Function* ts::sl::FUNC_DEF(entry::EntryType returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, size_t argumentCount, entry::EntryType* argumentTypes) {
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
	return new MethodTree(nameString, -1);
}

void ts::sl::define(Interpreter* interpreter) {
	// define namespaces and their inheristance structure
	vector<MethodTree*> methodTrees;

	MethodTree* SimObject = interpreter->createMethodTreeFromNamespace("SimObject");
	SimObject->isTSSL = true;
	methodTrees.push_back(SimObject);

	MethodTree* ScriptObject = interpreter->createMethodTreeFromNamespace("ScriptObject");
	ScriptObject->isTSSL = true;
	methodTrees.push_back(ScriptObject);
	ScriptObject->addParent(SimObject);

	for(MethodTree* tree: methodTrees) {
		interpreter->defineTSSLMethodTree(tree);
	}
	
	// define functions/methods
	vector<ts::sl::Function*> functions;
	entry::EntryType* s = new entry::EntryType[TS_ARG_COUNT] { entry::STRING };
	entry::EntryType* n = new entry::EntryType[TS_ARG_COUNT] { entry::NUMBER };
	entry::EntryType* o = new entry::EntryType[TS_ARG_COUNT] { entry::OBJECT };
	entry::EntryType* sn = new entry::EntryType[TS_ARG_COUNT] { entry::STRING, entry::NUMBER };
	entry::EntryType* nn = new entry::EntryType[TS_ARG_COUNT] { entry::NUMBER, entry::NUMBER };
	entry::EntryType* os = new entry::EntryType[TS_ARG_COUNT] { entry::OBJECT, entry::STRING };
	entry::EntryType* ns = new entry::EntryType[TS_ARG_COUNT] { entry::NUMBER, entry::STRING };
	entry::EntryType* snn = new entry::EntryType[TS_ARG_COUNT] { entry::STRING, entry::NUMBER, entry::NUMBER };
	entry::EntryType* ssn = new entry::EntryType[TS_ARG_COUNT] { entry::STRING, entry::STRING, entry::NUMBER };
	entry::EntryType* sns = new entry::EntryType[TS_ARG_COUNT] { entry::STRING, entry::NUMBER, entry::STRING };
	entry::EntryType* ons = new entry::EntryType[TS_ARG_COUNT] { entry::OBJECT, entry::NUMBER, entry::STRING };

	functions.push_back(FUNC_DEF(entry::INVALID, &echo, "echo", 1, s));
	functions.push_back(FUNC_DEF(entry::INVALID, &error, "error", 1, s));

	functions.push_back(FUNC_DEF(entry::STRING, &firstWord, "firstWord", 1, s));
	functions.push_back(FUNC_DEF(entry::STRING, &restWords, "restWords", 1, s));
	functions.push_back(FUNC_DEF(entry::STRING, &getWord, "getWord", 2, sn));
	functions.push_back(FUNC_DEF(entry::STRING, &getWords, "getWords", 3, snn));
	functions.push_back(FUNC_DEF(entry::NUMBER, &getWordCount, "getWordCount", 1, s));
	functions.push_back(FUNC_DEF(entry::STRING, &removeWord, "removeWord", 2, sn));
	functions.push_back(FUNC_DEF(entry::STRING, &setWord, "setWord", 3, sns));
	functions.push_back(FUNC_DEF(entry::STRING, &strLen, "strLen", 1, s));
	functions.push_back(FUNC_DEF(entry::STRING, &getSubStr, "getSubStr", 1, snn));
	functions.push_back(FUNC_DEF(entry::STRING, &strPos, "strPos", 3, ssn));
	functions.push_back(FUNC_DEF(entry::STRING, &trim, "trim", 1, s));

	functions.push_back(FUNC_DEF(entry::NUMBER, &mAbs, "mAbs", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mACos, "mACos", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mASin, "mASin", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mATan, "mATan", 2, nn));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mCeil, "mCeil", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mFloor, "mFloor", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mCos, "mCos", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mSin, "mSin", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mTan, "mTan", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mPow, "mPow", 2, nn));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mSqrt, "mSqrt", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mDegToRad, "mDegToRad", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mRadToDeg, "mRadToDeg", 1, n));
	functions.push_back(FUNC_DEF(entry::NUMBER, &mLog, "mLog", 1, n));
	functions.push_back(FUNC_DEF(entry::STRING, &mFloatLength, "mFloatLength", 2, n));

	functions.push_back(FUNC_DEF(entry::INVALID, &schedule, "schedule", 2, ns));
	functions.push_back(FUNC_DEF(entry::INVALID, &SimObject__schedule, "SimObject", "schedule", 3, ons));

	functions.push_back(FUNC_DEF(entry::INVALID, &SimObject__test, "SimObject", "test", 2, os));
	functions.push_back(FUNC_DEF(entry::INVALID, &ScriptObject__test, "ScriptObject", "test", 2, os));

	functions.push_back(FUNC_DEF(entry::INVALID, &isObject, "isObject", 1, o));

	for(ts::sl::Function* function: functions) {
		interpreter->defineTSSLFunction(function);
	}
}

Entry* ts::sl::PARENT(Interpreter* interpreter, const char* methodName, size_t argc, Entry* argv, entry::EntryType* argumentTypes) {
	string methodNameString(methodName);
	return interpreter->handleTSSLParent(methodNameString, argc, argv, argumentTypes);
}
