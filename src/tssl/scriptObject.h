#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		class ScriptObject : public Object {

		};

		Entry* ScriptObject__test(Interpreter* interpreter, size_t argc, Entry* args);
	}
}