#pragma once

typedef unsigned int stack_location;
typedef unsigned int relative_stack_location;

namespace ts {
	struct StackFrame {
		stack_location start;
		unsigned int size;
	};
}