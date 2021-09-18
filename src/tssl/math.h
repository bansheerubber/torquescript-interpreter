#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* mAbs(Engine* engine, unsigned int argc, Entry* args);
		Entry* mACos(Engine* engine, unsigned int argc, Entry* args);
		Entry* mASin(Engine* engine, unsigned int argc, Entry* args);
		Entry* mATan(Engine* engine, unsigned int argc, Entry* args);
		Entry* mCeil(Engine* engine, unsigned int argc, Entry* args);
		Entry* mFloor(Engine* engine, unsigned int argc, Entry* args);
		Entry* mCos(Engine* engine, unsigned int argc, Entry* args);
		Entry* mSin(Engine* engine, unsigned int argc, Entry* args);
		Entry* mTan(Engine* engine, unsigned int argc, Entry* args);
		Entry* mPow(Engine* engine, unsigned int argc, Entry* args);
		Entry* mSqrt(Engine* engine, unsigned int argc, Entry* args);
		Entry* mDegToRad(Engine* engine, unsigned int argc, Entry* args);
		Entry* mRadToDeg(Engine* engine, unsigned int argc, Entry* args);
		Entry* mLog(Engine* engine, unsigned int argc, Entry* args);
		Entry* mFloatLength(Engine* engine, unsigned int argc, Entry* args);

		Entry* getRandom(Engine* engine, unsigned int argc, Entry* args);
		Entry* setRandomSeed(Engine* engine, unsigned int argc, Entry* args);
		Entry* getRandomSeed(Engine* engine, unsigned int argc, Entry* args);
	}
}