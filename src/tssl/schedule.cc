#include "schedule.h"

#include "define.h"
#include "../interpreter/interpreter.h"
#include "../interpreter/objectReference.h"

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

		Entry* SimObject__schedule(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc < 3) {
				return nullptr;
			}
			else if(argc == 3) {
				string command(args[2].stringData);

				Entry* arguments = new Entry[1];
				copyEntry(args[0], arguments[0]);
				interpreter->addSchedule((size_t)args[1].numberData * 1000, command, arguments, 1, new ObjectReference(args[0].objectData));
				return nullptr;
			}

			Entry* copiedArguments = new Entry[argc - 2];
			copyEntry(args[0], copiedArguments[0]);
			for(size_t i = 0; i < argc - 3; i++) {
				copyEntry(args[i + 3], copiedArguments[i + 1]);
			}
			
			string command(args[2].stringData);
			interpreter->addSchedule((unsigned long long)args[1].numberData * 1000, command, copiedArguments, argc - 2, new ObjectReference(args[0].objectData));
			return nullptr;
		}
	}
}
