#pragma once

#include <vector>

#include "../interpreter/instruction.h"

using namespace std;

namespace ts {
	class LoopsContext {
		public:
			vector<InstructionReturn> breaks;
			vector<InstructionReturn> continues;
	};
}
