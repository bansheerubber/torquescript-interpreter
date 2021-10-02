#include "breakStatement.h"
#include "../interpreter/interpreter.h"

bool BreakStatement::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == BREAK;
}

BreakStatement* BreakStatement::Parse(Component* parent, ts::Engine* engine) {
	BreakStatement* output = new BreakStatement(engine);
	output->parent = parent;
	engine->parser->expectToken(BREAK);
	engine->parser->expectToken(SEMICOLON);
	return output;
}

string BreakStatement::print() {
	return "break;";
}

string BreakStatement::printJSON() {
	return "{\"type\":\"BREAK_STATEMENT\"}";
}

ts::InstructionReturn BreakStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	if(context.loop == nullptr) {
		engine->parser->error("break statement must be in loop body");
		return {};
	}
	
	ts::InstructionReturn output;
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::JUMP;
	output.add(instruction);
	context.loop->breaks.push_back(output);
	return output;
}
