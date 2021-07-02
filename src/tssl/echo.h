#pragma once

#include <string>

using namespace std;

namespace ts {
	namespace sl {
		void* echo(size_t argc, void** args) {
			if(argc >= 1) {
				printf("%s\n", ((const char*)args[0]));
			}

			return nullptr;
		}
	}
}