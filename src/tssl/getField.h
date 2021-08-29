#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Interpreter;

	namespace sl {
		Entry* firstField(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* restFields(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getField(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getFields(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getFieldCount(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* removeField(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* setField(Interpreter* interpreter, size_t argc, Entry* args);
	}
}