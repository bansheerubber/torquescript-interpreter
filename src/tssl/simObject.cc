#include "simObject.h"

#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		void* SimObject__test(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 2) {
				printf("attemtping print because argc high\n");
				printf("sim object says: %s\n", ((const char*)args[1]));
			}
			return nullptr;
		}
	}
}
