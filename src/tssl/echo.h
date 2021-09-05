#pragma once

#include <string>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* echo(Engine* engine, size_t argc, Entry* args);
		Entry* error(Engine* engine, size_t argc, Entry* args);

		extern string mockStdout;
		extern bool useMockStdout;
	}
}
