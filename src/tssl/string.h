#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		Entry* strLen(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getSubStr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strPos(Interpreter* interpreter, size_t argc, Entry* args);
	}
}
