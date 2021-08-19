#include "compiler.h"

using namespace ts;

Instruction* ts::Compile(Parser* parser, Interpreter* interpreter) {
	if(parser->errors.size() > 0) {
		exit(1);
	}
	
	InstructionReturn result = parser->getSourceFile()->compile(interpreter, {
		loop: nullptr,
		scope: nullptr,
	});
	return result.first;
}