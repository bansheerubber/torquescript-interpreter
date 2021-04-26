#include "postfixStatement.h"

bool PostfixStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	Token token = tokenizer->peekToken();
	return token.type == INCREMENT
		|| token.type == DECREMENT;
}

PostfixStatement* PostfixStatement::Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	PostfixStatement* output = new PostfixStatement();
	output->parent = parent;
	output->lvalue = lvalue;
	lvalue->setParent(output);
	output->op = parser->expectToken(INCREMENT, DECREMENT);
	return output;
}

string PostfixStatement::print() {
	string output = this->lvalue->print() + this->op.lexeme;
	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}