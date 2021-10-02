#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Engine;

	namespace sl {
		Entry* firstWord(Engine* engine, unsigned int argc, Entry* args);
		Entry* restWords(Engine* engine, unsigned int argc, Entry* args);
		Entry* getWord(Engine* engine, unsigned int argc, Entry* args);
		Entry* getWords(Engine* engine, unsigned int argc, Entry* args);
		Entry* getWordCount(Engine* engine, unsigned int argc, Entry* args);
		Entry* removeWord(Engine* engine, unsigned int argc, Entry* args);
		Entry* setWord(Engine* engine, unsigned int argc, Entry* args);
	}
}