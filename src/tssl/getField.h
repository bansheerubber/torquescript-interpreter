#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Engine;

	namespace sl {
		Entry* firstField(Engine* engine, unsigned int argc, Entry* args);
		Entry* restFields(Engine* engine, unsigned int argc, Entry* args);
		Entry* getField(Engine* engine, unsigned int argc, Entry* args);
		Entry* getFields(Engine* engine, unsigned int argc, Entry* args);
		Entry* getFieldCount(Engine* engine, unsigned int argc, Entry* args);
		Entry* removeField(Engine* engine, unsigned int argc, Entry* args);
		Entry* setField(Engine* engine, unsigned int argc, Entry* args);
	}
}