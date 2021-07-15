#include "echo.h"

#include "../interpreter/interpreter.h"

namespace ts {
	namespace sl {
		string mockStdout = string();
		bool useMockStdout = false;
		
		void* echo(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 1) {
				if(useMockStdout) {
					mockStdout += (const char*)args[0];
					mockStdout += '\n';
				}
				else {
					printf("%s\n", ((const char*)args[0]));
				}
			}
			return nullptr;
		}		
	}
}
