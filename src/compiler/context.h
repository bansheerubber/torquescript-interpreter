#pragma once

#include "loops.h"
#include "scope.h"

namespace ts {
	struct CompilationContext {
		LoopsContext* loop;
		Scope* scope;
	};
}
