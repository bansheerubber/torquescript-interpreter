#include "echo.h"

#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		string mockStdout = string();
		bool useMockStdout = false;
		
		Entry* echo(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 1) {
				if(useMockStdout) {
					mockStdout += args[0].stringData;
					mockStdout += '\n';
				}
				else {
					printf("%s\n", args[0].stringData);
				}
			}
			return nullptr;
		}
	}
}
