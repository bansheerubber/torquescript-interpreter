#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		class SimObject : public Object {

		};

		Entry* SimObject__test(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* SimObject__getId(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* SimObject__delete(Interpreter* interpreter, size_t argc, Entry* args);
	}
}