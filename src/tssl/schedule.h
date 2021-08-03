#pragma once

#include "../interpreter/entry.h"

namespace ts {
	class Interpreter;

	namespace sl {
		Entry* schedule(Interpreter* interpreter, size_t argc, Entry* args);

		Entry* SimObject__schedule(Interpreter* interpreter, size_t argc, Entry* args);
	}
}
