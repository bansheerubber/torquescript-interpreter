#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Interpreter;

	namespace sl {
		Entry* firstRecord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* restRecords(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getRecord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getRecords(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getRecordCount(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* removeRecord(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* setRecord(Interpreter* interpreter, size_t argc, Entry* args);
	}
}