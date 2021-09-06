#include "echo.h"

#include "../engine/engine.h"

namespace ts {
	namespace sl {
		string mockStdout = string();
		bool useMockStdout = false;
		
		Entry* echo(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				if(useMockStdout) {
					mockStdout += args[0].stringData;
					mockStdout += '\n';
				}
				else {
					(*engine->printFunction)("%s\n", args[0].stringData);
				}
			}
			return nullptr;
		}

		Entry* error(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				if(useMockStdout) {
					mockStdout += args[0].stringData;
					mockStdout += '\n';
				}
				else {
					(*engine->errorFunction)("%s\n", args[0].stringData);
				}
			}
			return nullptr;
		}
	}
}
