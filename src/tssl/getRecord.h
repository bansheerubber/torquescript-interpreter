#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	class Engine;

	namespace sl {
		Entry* firstRecord(Engine* engine, unsigned int argc, Entry* args);
		Entry* restRecords(Engine* engine, unsigned int argc, Entry* args);
		Entry* getRecord(Engine* engine, unsigned int argc, Entry* args);
		Entry* getRecords(Engine* engine, unsigned int argc, Entry* args);
		Entry* getRecordCount(Engine* engine, unsigned int argc, Entry* args);
		Entry* removeRecord(Engine* engine, unsigned int argc, Entry* args);
		Entry* setRecord(Engine* engine, unsigned int argc, Entry* args);
	}
}