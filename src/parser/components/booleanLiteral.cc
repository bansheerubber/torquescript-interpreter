#include "booleanLiteral.h"

bool BooleanLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == TRUE || tokenizer->peekToken().type == FALSE;
}

BooleanLiteral* BooleanLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	BooleanLiteral* output = new BooleanLiteral(parser);
	output->parent = parent;
	output->value = tokenizer->getToken();
	return output;
}

string BooleanLiteral::print() {
	if(this->value.type == TRUE) {
		return "true";
	}
	else {
		return "false";
	}
}