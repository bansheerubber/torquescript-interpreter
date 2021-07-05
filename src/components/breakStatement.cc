#include "breakStatement.h"
#include "../interpreter/interpreter.h"

bool BreakStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == BREAK;
}

BreakStatement* BreakStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	BreakStatement* output = new BreakStatement(parser);
	output->parent = parent;
	parser->expectToken(BREAK);
	parser->expectToken(SEMICOLON);
	return output;
}

string BreakStatement::print() {
	return "break;";
}

string BreakStatement::printJSON() {
	return "{\"type\":\"BREAK_STATEMENT\"}";
}

ts::InstructionReturn BreakStatement::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	if(context.loop == nullptr) {
		this->parser->error("break statement must be in loop body");
		return {};
	}
	
	ts::InstructionReturn output;
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::JUMP;
	output.add(instruction);
	context.loop->breaks.push_back(output);
	return output;
}