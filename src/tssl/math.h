#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Interpreter;
	
	namespace sl {
		Entry* mAbs(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mACos(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mASin(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mATan(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mCeil(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mFloor(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mCos(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mSin(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mTan(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mPow(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mSqrt(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mDegToRad(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mRadToDeg(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mLog(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* mFloatLength(Interpreter* interpreter, size_t argc, Entry* args);
	}
}