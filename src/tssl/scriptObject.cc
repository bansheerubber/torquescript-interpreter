#include "scriptObject.h"

#include "define.h"
#include "../engine/engine.h"

namespace ts {
	namespace sl {
		Entry* ScriptObject__test(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				entry::EntryType* types = new entry::EntryType[2] { entry::OBJECT, entry::STRING };
				PARENT(engine, "test", argc, args, types);
				
				printf("attemtping print because argc high\n");
				printf("script object says: %s\n", args[1].stringData);
			}
			return nullptr;
		}
	}
}
