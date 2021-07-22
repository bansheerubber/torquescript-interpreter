#pragma once

#include <string>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Interpreter;
	
	namespace sl {
		Entry* echo(Interpreter* interpreter, size_t argc, Entry* args);

		extern string mockStdout;
		extern bool useMockStdout;
	}
}
