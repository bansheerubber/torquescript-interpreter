#include "stringLiteral.h"

bool StringLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == STRING || tokenizer->peekToken().type == TAGGED_STRING;
}

StringLiteral* StringLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	StringLiteral* output = new StringLiteral();
	output->parent = parent;
	output->value = tokenizer->getToken();
	return output;
}

string StringLiteral::print() {
	if(this->value.type == STRING) {
		return "\"" + this->value.lexeme + "\"";
	}
	else {
		return "'" + this->value.lexeme + "'";
	}
}