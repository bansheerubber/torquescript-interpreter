#pragma once

#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		class ScriptObject : public Object {

		};

		void* ScriptObject__test(Interpreter* interpreter, size_t argc, void** args);
	}
}