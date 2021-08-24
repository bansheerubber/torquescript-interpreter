#include "isObject.h"

#include "../interpreter/interpreter.h"

using namespace std;

namespace ts {
	namespace sl {
		Entry* isObject(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				if(args[0].objectData->objectWrapper != nullptr) {
					return new Entry(1.0);
				}
				else {
					return new Entry(0.0);
				}
			}
			
			return nullptr;
		}
	}
}