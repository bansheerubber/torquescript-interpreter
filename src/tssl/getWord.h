#pragma once

#include <cstddef>

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Interpreter;

	namespace sl {
		void* firstWord(Interpreter* interpreter, size_t argc, void** args);
		void* restWords(Interpreter* interpreter, size_t argc, void** args);
		void* getWord(Interpreter* interpreter, size_t argc, void** args);
		void* getWords(Interpreter* interpreter, size_t argc, void** args);
		void* getWordCount(Interpreter* interpreter, size_t argc, void** args);
		void* removeWord(Interpreter* interpreter, size_t argc, void** args);
		void* setWord(Interpreter* interpreter, size_t argc, void** args);
	}
}