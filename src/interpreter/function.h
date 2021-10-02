#pragma once

#include "instructionContainer.h"
#include "../tssl/define.h"

namespace ts {
	class Function : public InstructionContainer {
		public:
			Function(Instruction* head, size_t argumentCount, size_t variableCount, string functionName, string namespaceName = string());
			Function(sl::Function* function);
			~Function();

			bool isTSSL = false;
			sl::Function* function = nullptr; // the standard library function to call if we're a standard library function
			string functionName;
			string namespaceName;
			size_t argumentCount;
			size_t variableCount;
			bool isActive = true;
			bool isPackaged;
	};
}
