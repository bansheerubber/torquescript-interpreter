#include "continueStatement.h"

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

ts::InstructionReturn ContinueStatement::compile() {
	return {};
}
