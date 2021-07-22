#include "simObject.h"

#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		Entry* SimObject__test(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 2) {
				printf("attemtping print because argc high\n");
				printf("sim object says: %s\n", args[1].stringData);
			}
			return nullptr;
		}
	}
}
