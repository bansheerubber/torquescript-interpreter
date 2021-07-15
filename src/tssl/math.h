#pragma once

#include <cstddef>

using namespace std;

namespace ts {
	class Interpreter;
	
	namespace sl {
		void* mAbs(Interpreter* interpreter, size_t argc, void** args);
		void* mACos(Interpreter* interpreter, size_t argc, void** args);
		void* mASin(Interpreter* interpreter, size_t argc, void** args);
		void* mATan(Interpreter* interpreter, size_t argc, void** args);
		void* mCeil(Interpreter* interpreter, size_t argc, void** args);
		void* mFloor(Interpreter* interpreter, size_t argc, void** args);
		void* mCos(Interpreter* interpreter, size_t argc, void** args);
		void* mSin(Interpreter* interpreter, size_t argc, void** args);
		void* mTan(Interpreter* interpreter, size_t argc, void** args);
		void* mPow(Interpreter* interpreter, size_t argc, void** args);
		void* mSqrt(Interpreter* interpreter, size_t argc, void** args);
		void* mDegToRad(Interpreter* interpreter, size_t argc, void** args);
		void* mRadToDeg(Interpreter* interpreter, size_t argc, void** args);
		void* mLog(Interpreter* interpreter, size_t argc, void** args);
		void* mFloatLength(Interpreter* interpreter, size_t argc, void** args);
	}
}