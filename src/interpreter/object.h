#pragma once

#include "variableContext.h"

using namespace ts;

namespace ts {
	class Object {
		public:
			Object(class ts::Interpreter* interpreter);

			VariableContext properties;
			unsigned int id = 0;
	};
}
