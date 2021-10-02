#pragma once

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* exec(Engine* engine, unsigned int argc, Entry* args);
	}
}
