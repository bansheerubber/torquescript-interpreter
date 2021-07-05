#include "compiler.h"

using namespace ts;

Instruction* ts::Compile(Parser* parser, Interpreter* interpreter) {
	InstructionReturn result = parser->getSourceFile()->compile(interpreter, {
		loop: nullptr,
		scope: nullptr,
	});
	return result.first;
}