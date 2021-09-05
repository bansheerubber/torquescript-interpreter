#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Engine;

	namespace sl {
		Entry* firstField(Engine* engine, size_t argc, Entry* args);
		Entry* restFields(Engine* engine, size_t argc, Entry* args);
		Entry* getField(Engine* engine, size_t argc, Entry* args);
		Entry* getFields(Engine* engine, size_t argc, Entry* args);
		Entry* getFieldCount(Engine* engine, size_t argc, Entry* args);
		Entry* removeField(Engine* engine, size_t argc, Entry* args);
		Entry* setField(Engine* engine, size_t argc, Entry* args);
	}
}