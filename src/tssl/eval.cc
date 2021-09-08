#include "eval.h"

#include "../engine/engine.h"

namespace ts {
	namespace sl {
		Entry* eval(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				string shell(args[0].stringData);
				engine->execShell(shell, true);
			}
			
			return nullptr;
		}
	}
}
