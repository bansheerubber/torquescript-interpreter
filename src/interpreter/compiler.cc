#include "compiler.h"

using namespace ts;

Instruction* ts::Compile(Parser* parser) {
	InstructionReturn result = parser->getSourceFile()->compile();
	return result.first;
}