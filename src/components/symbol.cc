#include "symbol.h"
#include "../interpreter/interpreter.h"

#include "../util/allocateString.h"

bool Symbol::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == SYMBOL;
}

bool Symbol::ShouldParseAlphabeticToken(ts::Engine* engine) {
	Token &token = engine->tokenizer->peekToken();
	
	return token.type == RETURN
		|| token.type == PACKAGE
		|| token.type == NEW
		|| token.type == FUNCTION
		|| token.type == IF
		|| token.type == ELSE
		|| token.type == WHILE
		|| token.type == FOR
		|| token.type == SWITCH
		|| token.type == CASE
		|| token.type == OR
		|| token.type == DEFAULT
		|| token.type == PARENT
		|| token.type == CONTINUE
		|| token.type == BREAK
		|| token.type == DATABLOCK
		|| token.type == TRUE
		|| token.type == FALSE
		|| token.type == SPC
		|| token.type == TAB
		|| token.type == NL;
}

Symbol* Symbol::Parse(Component* parent, ts::Engine* engine) {
	Symbol* output = new Symbol(engine);
	output->parent = parent;
	output->value = engine->tokenizer->getToken().lexeme;
	return output;
}

string Symbol::print() {
	return this->value;
}

string Symbol::printJSON() {
	return "{\"type\":\"SYMBOL_STATEMENT\",\"value\":\"" + this->value + "\"}";
}

ts::InstructionReturn Symbol::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::SYMBOL_ACCESS;
	instruction->symbolAccess.dimensions = 0;
	instruction->symbolAccess.hash = hash<string>{}(this->value);
	ALLOCATE_STRING(this->value, instruction->symbolAccess.source);
	return ts::InstructionReturn(instruction, instruction);
}
