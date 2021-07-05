#include "symbol.h"
#include "../interpreter/interpreter.h"

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

string Symbol::printJSON() {
	return "{\"type\":\"SYMBOL_STATEMENT\",\"value\":\"" + this->value + "\"}";
}

ts::InstructionReturn Symbol::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	this->parser->error("%s not supported", this->parser->typeToName(this->getType()));
	return {};
}
