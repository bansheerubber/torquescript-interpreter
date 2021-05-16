#include "inheritanceStatement.h"
#include "../interpreter/interpreter.h"

bool InheritanceStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		(
			tokenizer->peekToken().type == LEFT_PARENTHESIS
			&& tokenizer->peekToken(1).type == SYMBOL
			&& tokenizer->peekToken(2).type == RIGHT_PARENTHESIS
		)
		|| (
			tokenizer->peekToken().type == LEFT_PARENTHESIS
			&& tokenizer->peekToken(1).type == SYMBOL
			&& tokenizer->peekToken(2).type == COLON
			&& tokenizer->peekToken(3).type == SYMBOL
			&& tokenizer->peekToken(4).type == RIGHT_PARENTHESIS
		)
		|| (
			tokenizer->peekToken().type == LEFT_PARENTHESIS
			&& tokenizer->peekToken(1).type == COLON
			&& tokenizer->peekToken(2).type == SYMBOL
			&& tokenizer->peekToken(3).type == RIGHT_PARENTHESIS
		)
	);
}

InheritanceStatement* InheritanceStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	InheritanceStatement* output = new InheritanceStatement(parser);
	output->parent = parent;
	
	parser->expectToken(LEFT_PARENTHESIS);

	bool nameSpecified = false;
	if(Symbol::ShouldParse(tokenizer, parser)) {
		output->className = Symbol::Parse(output, tokenizer, parser);
		nameSpecified = true;
	}

	if(tokenizer->peekToken().type == COLON) {
		parser->expectToken(COLON);
		output->parentClass = Symbol::Parse(output, tokenizer, parser);
	}
	else if(!nameSpecified) {
		parser->error("unexpected statement in new object name");
	}

	parser->expectToken(RIGHT_PARENTHESIS);
	return output;
}

string InheritanceStatement::print() {
	if(this->className == nullptr && this->parentClass != nullptr) {
		return "(" + this->parser->space + ":" + this->parser->space + this->parentClass->print() + ")";
	}
	else if(this->parentClass != nullptr) {
		return "(" + this->className->print() + this->parser->space + ":" + this->parser->space + this->parentClass->print() + ")";
	}
	return "(" + this->className->print() + ")";
}

ts::InstructionReturn InheritanceStatement::compile(ts::Interpreter* interpreter) {
	return {};
}
