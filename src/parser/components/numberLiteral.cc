#include "numberLiteral.h"

bool NumberLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == NUMBER;
}

NumberLiteral* NumberLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NumberLiteral* output = new NumberLiteral(parser);
	output->parent = parent;
	output->number = tokenizer->getToken().lexeme;
	return output;
}

string NumberLiteral::print() {
	return this->number;
}