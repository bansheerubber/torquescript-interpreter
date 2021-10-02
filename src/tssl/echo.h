#pragma once

#include <string>

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* echo(Engine* engine, unsigned int argc, Entry* args);
		Entry* error(Engine* engine, unsigned int argc, Entry* args);

		extern string mockStdout;
		extern bool useMockStdout;
	}
}
