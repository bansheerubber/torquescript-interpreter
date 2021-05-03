#include "symbol.h"

bool Symbol::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == SYMBOL;
}

Symbol* Symbol::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	Symbol* output = new Symbol(parser);
	output->parent = parent;
	output->value = tokenizer->getToken().lexeme;
	return output;
}

string Symbol::print() {
	return this->value;
}

ts::InstructionReturn Symbol::compile() {
	return {};
}
