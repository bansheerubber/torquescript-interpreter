#pragma once

#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		class SimObject : public Object {

		};

		void* SimObject__test(Interpreter* interpreter, size_t argc, void** args);
	}
}