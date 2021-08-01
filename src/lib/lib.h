#pragma once

#include "libSymbols.h"

#include "../util/dynamicArray.h"

namespace ts {
	class Interpreter;
}

void initInterpreterId(void* nothing, ts::Interpreter** value);

extern DynamicArray<ts::Interpreter*, void> interpreters;