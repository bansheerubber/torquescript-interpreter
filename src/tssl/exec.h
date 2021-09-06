#pragma once

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* exec(Engine* engine, size_t argc, Entry* args);
	}
}
