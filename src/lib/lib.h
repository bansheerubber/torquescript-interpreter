#pragma once

#include "libSymbols.h"

#include "../interpreter/dynamicArray.h"

namespace ts {
	class Interpreter;
}

void initInterpreterId(void* nothing, ts::Interpreter** value);

extern ts::DynamicArray<ts::Interpreter*, void> interpreters;