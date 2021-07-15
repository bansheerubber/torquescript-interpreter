#pragma once

#include <string>

using namespace std;

namespace ts {
	class Interpreter;
	
	namespace sl {
		void* echo(Interpreter* interpreter, size_t argc, void** args);

		extern string mockStdout;
		extern bool useMockStdout;
	}
}
