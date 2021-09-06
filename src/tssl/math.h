#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* mAbs(Engine* engine, size_t argc, Entry* args);
		Entry* mACos(Engine* engine, size_t argc, Entry* args);
		Entry* mASin(Engine* engine, size_t argc, Entry* args);
		Entry* mATan(Engine* engine, size_t argc, Entry* args);
		Entry* mCeil(Engine* engine, size_t argc, Entry* args);
		Entry* mFloor(Engine* engine, size_t argc, Entry* args);
		Entry* mCos(Engine* engine, size_t argc, Entry* args);
		Entry* mSin(Engine* engine, size_t argc, Entry* args);
		Entry* mTan(Engine* engine, size_t argc, Entry* args);
		Entry* mPow(Engine* engine, size_t argc, Entry* args);
		Entry* mSqrt(Engine* engine, size_t argc, Entry* args);
		Entry* mDegToRad(Engine* engine, size_t argc, Entry* args);
		Entry* mRadToDeg(Engine* engine, size_t argc, Entry* args);
		Entry* mLog(Engine* engine, size_t argc, Entry* args);
		Entry* mFloatLength(Engine* engine, size_t argc, Entry* args);
	}
}