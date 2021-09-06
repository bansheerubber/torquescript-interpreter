#include "schedule.h"

#include "define.h"
#include "../engine/engine.h"
#include "../interpreter/objectReference.h"

namespace ts {
	namespace sl {
		Entry* schedule(Engine* engine, size_t argc, Entry* args) {
			if(argc < 2) {
				return nullptr;
			}
			else if(argc == 2) {
				string functionName(args[1].stringData);
				engine->interpreter->addSchedule((unsigned long long)args[0].numberData * 1000, functionName, nullptr, 0);
				return nullptr;
			}

			Entry* copiedArguments = new Entry[argc - 2];
			for(size_t i = 0; i < argc - 2; i++) {
				copyEntry(args[i + 2], copiedArguments[i]);
			}
			
			string functionName(args[1].stringData);
			engine->interpreter->addSchedule((unsigned long long)args[0].numberData * 1000, functionName, copiedArguments, argc - 2);
			return nullptr;
		}

		Entry* SimObject__schedule(Engine* engine, size_t argc, Entry* args) {
			if(argc < 3) {
				return nullptr;
			}
			else if(argc == 3) {
				string functionName(args[2].stringData);

				Entry* arguments = new Entry[1];
				copyEntry(args[0], arguments[0]);
				engine->interpreter->addSchedule((unsigned long long)args[1].numberData * 1000, functionName, arguments, 1, new ObjectReference(args[0].objectData));
				return nullptr;
			}

			Entry* copiedArguments = new Entry[argc - 2];
			copyEntry(args[0], copiedArguments[0]);
			for(size_t i = 0; i < argc - 3; i++) {
				copyEntry(args[i + 3], copiedArguments[i + 1]);
			}
			
			string functionName(args[2].stringData);
			engine->interpreter->addSchedule((unsigned long long)args[1].numberData * 1000, functionName, copiedArguments, argc - 2, new ObjectReference(args[0].objectData));
			return nullptr;
		}
	}
}
