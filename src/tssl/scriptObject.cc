#include "scriptObject.h"

#include "define.h"
#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		void* ScriptObject__test(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 2) {
				PARENT(interpreter, "test", argc, args);
				
				printf("attemtping print because argc high\n");
				printf("script object says: %s\n", ((const char*)args[1]));
			}
			return nullptr;
		}
	}
}
