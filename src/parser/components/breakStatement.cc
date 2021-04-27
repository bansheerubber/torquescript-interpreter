#include "breakStatement.h"

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