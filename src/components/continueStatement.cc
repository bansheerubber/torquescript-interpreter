#include "continueStatement.h"
#include "../interpreter/interpreter.h"

bool ContinueStatement::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == CONTINUE;
}

ContinueStatement* ContinueStatement::Parse(Component* parent, ts::Engine* engine) {
	ContinueStatement* output = new ContinueStatement(engine);
	output->parent = parent;
	engine->parser->expectToken(CONTINUE);
	engine->parser->expectToken(SEMICOLON);
	return output;
}

string ContinueStatement::print() {
	return "continue;";
}

string ContinueStatement::printJSON() {
	return "{\"type\":\"CONTINUE_STATEMENT\"}";
}

ts::InstructionReturn ContinueStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	if(context.loop == nullptr) {
		this->engine->parser->error("continue statement must be in loop body");
		return {};
	}
	
	ts::InstructionReturn output;
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::JUMP;
	output.add(instruction);
	context.loop->continues.push_back(output);
	return output;
}
