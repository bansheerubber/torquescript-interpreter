#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Engine;

	namespace sl {
		Entry* firstWord(Engine* engine, size_t argc, Entry* args);
		Entry* restWords(Engine* engine, size_t argc, Entry* args);
		Entry* getWord(Engine* engine, size_t argc, Entry* args);
		Entry* getWords(Engine* engine, size_t argc, Entry* args);
		Entry* getWordCount(Engine* engine, size_t argc, Entry* args);
		Entry* removeWord(Engine* engine, size_t argc, Entry* args);
		Entry* setWord(Engine* engine, size_t argc, Entry* args);
	}
}