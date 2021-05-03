#pragma once

#include "instruction.h"
#include "../parser/parser.h"

namespace ts {
	Instruction* Compile(Parser* parser);
}