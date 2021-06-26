#pragma once

#include <string>

using namespace std;

namespace ts {
	namespace sl {
		void* echo(size_t argc, void** args) {
			if(argc >= 1) {
				printf("%s\n", ((string*)args[0])->c_str());
			}

			return nullptr;
		}
	}
}