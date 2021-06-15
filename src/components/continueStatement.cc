#include "continueStatement.h"
#include "../interpreter/interpreter.h"

bool ContinueStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == CONTINUE;
}

ContinueStatement* ContinueStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	ContinueStatement* output = new ContinueStatement(parser);
	output->parent = parent;
	parser->expectToken(CONTINUE);
	parser->expectToken(SEMICOLON);
	return output;
}

string ContinueStatement::print() {
	return "continue;";
}

string ContinueStatement::printJSON() {
	return "{\"type\":\"CONTINUE_STATEMENT\"}";
}

ts::InstructionReturn ContinueStatement::compile(ts::Interpreter* interpreter) {
	return {};
}
