#include "inheritanceStatement.h"

bool InheritanceStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		tokenizer->peekToken().type == LEFT_PARENTHESIS
		&& tokenizer->peekToken(1).type == SYMBOL
		&& (
			tokenizer->peekToken(2).type == RIGHT_PARENTHESIS
			|| (
				tokenizer->peekToken(2).type == COLON
				&& tokenizer->peekToken(3).type == SYMBOL
				&& tokenizer->peekToken(4).type == RIGHT_PARENTHESIS
			)
		)
	);
}

InheritanceStatement* InheritanceStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	InheritanceStatement* output = new InheritanceStatement();
	output->parent = parent;
	
	parser->expectToken(LEFT_PARENTHESIS);
	output->className = Symbol::Parse(output, tokenizer, parser);

	if(tokenizer->peekToken().type == COLON) {
		parser->expectToken(COLON);
		output->parentClass = Symbol::Parse(output, tokenizer, parser);
	}
	parser->expectToken(RIGHT_PARENTHESIS);
	return output;
}

string InheritanceStatement::print() {
	if(this->parentClass != nullptr) {
		return "(" + this->className->print() + " : " + this->parentClass->print() + ")";
	}
	return "(" + this->className->print() + ")";
}