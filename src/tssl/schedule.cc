#include "schedule.h"

#include "define.h"
#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		Entry* schedule(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc < 2) {
				return nullptr;
			}
			else if(argc == 2) {
				string command(args[1].stringData);
				interpreter->addSchedule((size_t)args[0].numberData * 1000, command, nullptr, 0);
				return nullptr;
			}

			Entry* copiedArguments = new Entry[argc - 2];
			for(size_t i = 0; i < argc - 2; i++) {
				copyEntry(args[i + 2], copiedArguments[i]);
			}
			
			string command(args[1].stringData);
			interpreter->addSchedule((unsigned long long)args[0].numberData * 1000, command, copiedArguments, argc - 2);
			return nullptr;
		}
	}
}
