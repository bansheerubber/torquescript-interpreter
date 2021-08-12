#include "inheritanceStatement.h"
#include "../interpreter/interpreter.h"

bool InheritanceStatement::ShouldParse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	if(
		parent->parent == nullptr
		|| (
			parent->parent->getType() != NEW_STATEMENT
			&& parent->parent->getType() != DATABLOCK_DECLARATION
		)
	) {
		return false;
	}
	
	if(lvalue == nullptr) {
		return tokenizer->peekToken(0).type == COLON && tokenizer->peekToken(1).type == SYMBOL;
	}
	else {
		return (
			(
				lvalue->getType() == STRING_LITERAL
				|| lvalue->getType() == SYMBOL_STATEMENT
				|| lvalue->getType() == MATH_EXPRESSION
				|| lvalue->getType() == ACCESS_STATEMENT
			)
			&& tokenizer->peekToken(0).type == COLON && tokenizer->peekToken(1).type == SYMBOL
		);
	}
}

InheritanceStatement* InheritanceStatement::Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	InheritanceStatement* output = new InheritanceStatement(parser);
	output->parent = parent;
	output->className = lvalue;

	if(
		parent->parent == nullptr
		|| (
			parent->parent->getType() != NEW_STATEMENT
			&& parent->parent->getType() != DATABLOCK_DECLARATION
		)
	) {
		parser->error("invalid place for inheritance statement");
	}

	if(tokenizer->peekToken().type == COLON) {
		parser->expectToken(COLON);
		output->parentClass = Symbol::Parse(output, tokenizer, parser);
	}

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

string InheritanceStatement::printJSON() {
	if(this->className == nullptr && this->parentClass != nullptr) {
		return "{\"type\":\"INHERITANCE_STATEMENT\",\"parentClass\":" + this->parentClass->printJSON() + "}";
	}
	else if(this->parentClass != nullptr) {
		return "{\"type\":\"INHERITANCE_STATEMENT\",\"className\":" + this->className->printJSON() + ",\"parentClass\":" + this->parentClass->printJSON() + "}";
	}
	return "{\"type\":\"INHERITANCE_STATEMENT\",\"className\":" + this->className->printJSON() + "}";
}

ts::InstructionReturn InheritanceStatement::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	this->parser->error("%s not supported", this->parser->typeToName(this->getType()));
	return {};
}
