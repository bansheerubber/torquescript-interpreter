#include "assignStatement.h"
#include "accessStatement.h"

bool AssignStatement::ShouldParse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	Token token = tokenizer->peekToken();
	return (
		token.type == PLUS_ASSIGN
		|| token.type == MINUS_ASSIGN
		|| token.type == SLASH_ASSIGN
		|| token.type == ASTERISK_ASSIGN
		|| token.type == OR_ASSIGN
		|| token.type == AND_ASSIGN
		|| token.type == XOR_ASSIGN
		|| token.type == SHIFT_LEFT_ASSIGN
		|| token.type == SHIFT_RIGHT_ASSIGN
		|| token.type == MODULUS_ASSIGN
		|| token.type == ASSIGN
	) && (
		parent->getType() == DATABLOCK_DECLARATION
		|| parent->getType() == NEW_STATEMENT
		|| (
			(lvalue != nullptr && lvalue->IsValidLvalue()) || lvalue == nullptr
		)
	);
}

AssignStatement* AssignStatement::Parse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	AssignStatement* output = new AssignStatement(parser);
	output->parent = parent;
	lvalue->setParent(output);
	output->lvalue = lvalue;
	output->assignmentToken = tokenizer->getToken();

	// parse access statement first since we need to absorb it before checking for math
	if(Component::ShouldParse(output, tokenizer, parser)) {
		output->rvalue = Component::Parse(output, tokenizer, parser);
	}
	else {
		parser->error("could not find valid rvalue for assign");
	}

	return output;
}

string AssignStatement::print() {
	string output = this->lvalue->print() + this->parser->space + this->assignmentToken.lexeme + this->parser->space + this->rvalue->print();
	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}

ts::InstructionReturn AssignStatement::compile() {
	return {};
}
