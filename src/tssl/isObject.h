#pragma once

#include <cstddef>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Interpreter;
	
	namespace sl {
		Entry* isObject(Interpreter* interpreter, size_t argc, Entry* args);
	}
}
