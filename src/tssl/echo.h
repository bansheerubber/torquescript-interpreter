#pragma once

#include <string>

using namespace std;

namespace ts {
	namespace sl {
		void* echo(size_t argc, void** args);

		extern string mockStdout;
		extern bool useMockStdout;
	}
}
