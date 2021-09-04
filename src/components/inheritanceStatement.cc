#include "inheritanceStatement.h"
#include "../interpreter/interpreter.h"

bool InheritanceStatement::ShouldParse(Component* lvalue, Component* parent, ts::Engine* engine) {
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
		return engine->tokenizer->peekToken(0).type == COLON && engine->tokenizer->peekToken(1).type == SYMBOL;
	}
	else {
		return (
			(
				lvalue->getType() == STRING_LITERAL
				|| lvalue->getType() == SYMBOL_STATEMENT
				|| lvalue->getType() == MATH_EXPRESSION
				|| lvalue->getType() == ACCESS_STATEMENT
			)
			&& engine->tokenizer->peekToken(0).type == COLON && engine->tokenizer->peekToken(1).type == SYMBOL
		);
	}
}

InheritanceStatement* InheritanceStatement::Parse(Component* lvalue, Component* parent, ts::Engine* engine) {
	InheritanceStatement* output = new InheritanceStatement(engine);
	output->parent = parent;
	output->className = lvalue;

	if(
		parent->parent == nullptr
		|| (
			parent->parent->getType() != NEW_STATEMENT
			&& parent->parent->getType() != DATABLOCK_DECLARATION
		)
	) {
		engine->parser->error("invalid place for inheritance statement");
	}

	if(engine->tokenizer->peekToken().type == COLON) {
		engine->parser->expectToken(COLON);
		output->parentClass = Symbol::Parse(output, engine);
	}

	return output;
}

string InheritanceStatement::print() {
	if(this->className == nullptr && this->parentClass != nullptr) {
		return "(" + this->engine->parser->space + ":" + this->engine->parser->space + this->parentClass->print() + ")";
	}
	else if(this->parentClass != nullptr) {
		return "(" + this->className->print() + this->engine->parser->space + ":" + this->engine->parser->space + this->parentClass->print() + ")";
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

ts::InstructionReturn InheritanceStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	this->engine->parser->error("%s not supported", this->engine->parser->typeToName(this->getType()));
	return {};
}
