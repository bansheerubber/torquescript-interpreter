#pragma once

#include <string>

using namespace std;

namespace ts {
	namespace functions {
		void echo(int argc, void** args) {
			if(argc >= 1) {
				printf("%s\n", ((string*)args[0])->c_str());
			}
		}
	}
}