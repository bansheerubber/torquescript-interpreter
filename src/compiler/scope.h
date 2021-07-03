#pragma once

#include "../include/robin-map/include/tsl/robin_map.h"
#include <string>

using namespace std;
using namespace tsl;

namespace ts {
	struct BoundVariable {
		size_t stackIndex; // index relative to start of stack frame
		string name;
	};
	
	class Scope {
		public:
			BoundVariable& allocateVariable(string &variableName);
		
		protected:
			size_t stackIndex = 0;
			robin_map<string, BoundVariable> variables;
	};
}
