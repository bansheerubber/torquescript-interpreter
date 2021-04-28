#include "returnStatement.h"

bool ReturnStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == RETURN;
}

ReturnStatement* ReturnStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	ReturnStatement* output = new ReturnStatement(parser);
	output->parent = parent;
	parser->expectToken(RETURN);

	// parse an expression
	if(tokenizer->peekToken().type != SEMICOLON) {
		if(!Component::ShouldParse(output, tokenizer, parser)) {
			parser->error("need valid expression for 'return' statement");
		}
		output->operation = Component::Parse(output, tokenizer, parser);

		parser->expectToken(SEMICOLON);
	}
	else {
		tokenizer->getToken(); // absorb semicolon
	}

	return output;
}

string ReturnStatement::print() {
	if(this->operation != nullptr) {
		return "return " + this->operation->print() + ";";
	}
	else {
		return "return;";
	}
}