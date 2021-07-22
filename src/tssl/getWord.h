#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Interpreter;

	namespace sl {
		Entry* firstWord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* restWords(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getWord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getWords(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getWordCount(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* removeWord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* setWord(Interpreter* interpreter, size_t argc, Entry* args);
	}
}