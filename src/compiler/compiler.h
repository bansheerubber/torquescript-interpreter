#pragma once

#include "../interpreter/instruction.h"
#include "../parser/parser.h"

namespace ts {
	Instruction* Compile(Parser* parser, Interpreter* interpreter);
}