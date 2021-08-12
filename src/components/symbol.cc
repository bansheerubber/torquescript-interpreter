#include "symbol.h"
#include "../interpreter/interpreter.h"

#include "../util/allocateString.h"

bool Symbol::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == SYMBOL;
}

bool Symbol::ShouldParseAlphabeticToken(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == RETURN
		|| tokenizer->peekToken().type == PACKAGE
		|| tokenizer->peekToken().type == NEW
		|| tokenizer->peekToken().type == FUNCTION
		|| tokenizer->peekToken().type == IF
		|| tokenizer->peekToken().type == ELSE
		|| tokenizer->peekToken().type == WHILE
		|| tokenizer->peekToken().type == FOR
		|| tokenizer->peekToken().type == SWITCH
		|| tokenizer->peekToken().type == CASE
		|| tokenizer->peekToken().type == OR
		|| tokenizer->peekToken().type == DEFAULT
		|| tokenizer->peekToken().type == PARENT
		|| tokenizer->peekToken().type == CONTINUE
		|| tokenizer->peekToken().type == BREAK
		|| tokenizer->peekToken().type == DATABLOCK
		|| tokenizer->peekToken().type == TRUE
		|| tokenizer->peekToken().type == FALSE
		|| tokenizer->peekToken().type == SPC
		|| tokenizer->peekToken().type == TAB
		|| tokenizer->peekToken().type == NL;
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
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::SYMBOL_ACCESS;
	instruction->symbolAccess.dimensions = 0;
	instruction->symbolAccess.hash = hash<string>{}(this->value);
	ALLOCATE_STRING(this->value, instruction->symbolAccess.source);
	return ts::InstructionReturn(instruction, instruction);
}
