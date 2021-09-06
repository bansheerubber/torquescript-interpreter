#include "eval.h"

#include "../engine/engine.h"

namespace ts {
	namespace sl {
		Entry* exec(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				string fileName(args[0].stringData);
				engine->execFile(fileName);
			}
			
			return nullptr;
		}
	}
}
