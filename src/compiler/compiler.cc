#include "compiler.h"

#include "../engine/engine.h"
#include "../components/sourceFile.h"

using namespace ts;

Instruction* Compile(Parser* parser, Engine* engine) {
	InstructionReturn result = parser->getSourceFile()->compile(engine, {
		loop: nullptr,
		scope: nullptr,
	});
	return result.first;
}