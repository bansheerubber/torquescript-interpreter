#include "define.h"

#include <math.h>

#include "echo.h"
#include "getWord.h"
#include "math.h"
#include "scriptObject.h"

using namespace ts::sl;

vector<Function*> ts::sl::functions;
unordered_map<string, unsigned long> ts::sl::nameToIndex;

void ts::sl::FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, int argumentCount, type* argumentTypes) {
	Function* function = new Function;
	function->returnType = returnType;
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;
	string stringName = string(name);
	nameToIndex[toLower(stringName)] = functions.size();
	functions.push_back(function);
}

void ts::sl::FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, int argumentCount, type* argumentTypes) {
	Function* function = new Function;
	function->returnType = returnType;
	function->nameSpace = string(nameSpace);
	function->name = string(name);
	function->argumentCount = argumentCount;
	function->function = functionPointer;
	function->argumentTypes = argumentTypes;
	string stringName = string(nameSpace) + "::" + string(name); // TODO improve this lookup table design
	nameToIndex[toLower(stringName)] = functions.size();
	functions.push_back(function);
}

void ts::sl::define() {
	type* s = new type[TS_ARG_COUNT] { type::STRING };
	type* n = new type[TS_ARG_COUNT] { type::NUMBER };
	type* sn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER };
	type* nn = new type[TS_ARG_COUNT] { type::NUMBER, type::NUMBER };
	type* snn = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::NUMBER };
	type* sns = new type[TS_ARG_COUNT] { type::STRING, type::NUMBER, type::STRING };
	type* o = new type[TS_ARG_COUNT] { type::OBJECT };

	FUNC_DEF(type::VOID, &echo, "echo", 1, s);

	FUNC_DEF(type::STRING, &firstWord, "firstWord", 1, s);
	FUNC_DEF(type::STRING, &restWords, "restWords", 1, s);
	FUNC_DEF(type::STRING, &getWord, "getWord", 2, sn);
	FUNC_DEF(type::STRING, &getWords, "getWords", 3, snn);
	FUNC_DEF(type::NUMBER, &getWordCount, "getWordCount", 1, s);
	FUNC_DEF(type::STRING, &removeWord, "removeWord", 2, sn);
	FUNC_DEF(type::STRING, &setWord, "setWord", 3, sns);

	FUNC_DEF(type::NUMBER, &mAbs, "mAbs", 1, n);
	FUNC_DEF(type::NUMBER, &mACos, "mACos", 1, n);
	FUNC_DEF(type::NUMBER, &mATan, "mATan", 2, nn);
	FUNC_DEF(type::NUMBER, &mCeil, "mCeil", 1, n);
	FUNC_DEF(type::NUMBER, &mFloor, "mFloor", 1, n);
	FUNC_DEF(type::NUMBER, &mCos, "mCos", 1, n);
	FUNC_DEF(type::NUMBER, &mSin, "mSin", 1, n);
	FUNC_DEF(type::NUMBER, &mTan, "mTan", 1, n);
	FUNC_DEF(type::NUMBER, &mPow, "mPow", 2, nn);
	FUNC_DEF(type::NUMBER, &mSqrt, "mSqrt", 1, n);
	FUNC_DEF(type::NUMBER, &mDegToRad, "mDegToRad", 1, n);
	FUNC_DEF(type::NUMBER, &mRadToDeg, "mRadToDeg", 1, n);
	FUNC_DEF(type::NUMBER, &mLog, "mLog", 1, n);
	FUNC_DEF(type::STRING, &mFloatLength, "mFloatLength", 2, n);

	FUNC_DEF(type::VOID, &ScriptObject__test, "ScriptObject", "test", 1, o);
}