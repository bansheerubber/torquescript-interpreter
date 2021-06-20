#pragma once

#include "instructionContainer.h"
#include "../tssl/define.h"

namespace ts {
	class Function : public InstructionContainer {
		public:
			Function(Instruction* head, string functionName, string namespaceName = string());
			Function(sl::Function* function);

			bool isTSSL = false;
			sl::Function* function; // the standard library function to call if we're a standard library function
			string functionName;
			string namespaceName;
	};
}