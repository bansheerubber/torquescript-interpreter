#pragma once

#include "../interpreter/entry.h"

using namespace std;

namespace ts {
	class Engine;
	
	namespace sl {
		Entry* eval(Engine* engine, unsigned int argc, Entry* args);
	}
}
