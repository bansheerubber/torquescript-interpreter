#include "compiler.h"

using namespace ts;

Instruction* ts::Compile(Parser* parser, Interpreter* interpreter) {
	InstructionReturn result = parser->getSourceFile()->compile(interpreter, nullptr);
	return result.first;
}