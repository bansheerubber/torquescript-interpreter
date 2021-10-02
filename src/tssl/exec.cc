#include "eval.h"

#include "../engine/engine.h"

namespace ts {
	namespace sl {
		Entry* exec(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				string fileName(args[0].stringData);
				engine->execFile(fileName, true);
			}
			
			return nullptr;
		}
	}
}
