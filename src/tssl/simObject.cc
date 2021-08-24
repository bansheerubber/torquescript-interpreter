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

		Entry* SimObject__getId(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(args[0].objectData->id);
			}
			return nullptr;
		}

		Entry* SimObject__delete(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper != nullptr) {
				delete args[0].objectData->objectWrapper;
			}
			return nullptr;
		}
	}
}
