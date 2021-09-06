#include "simObject.h"

#include "../engine/engine.h"

namespace ts {
	namespace sl {
		Entry* SimObject__test(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				printf("attemtping print because argc high\n");
				printf("sim object says: %s\n", args[1].stringData);
			}
			return nullptr;
		}

		Entry* SimObject__getId(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(args[0].objectData->id);
			}
			return nullptr;
		}

		Entry* SimObject__delete(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper != nullptr) {
				delete args[0].objectData->objectWrapper;
			}
			return nullptr;
		}
	}
}
