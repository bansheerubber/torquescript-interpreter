#include "booleanLiteral.h"
#include "../interpreter/interpreter.h"

bool BooleanLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == TRUE || tokenizer->peekToken().type == FALSE;
}

BooleanLiteral* BooleanLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	BooleanLiteral* output = new BooleanLiteral(parser);
	output->parent = parent;
	output->value = tokenizer->getToken();
	return output;
}

string BooleanLiteral::print() {
	if(this->value.type == TRUE) {
		return "true";
	}
	else {
		return "false";
	}
}

string BooleanLiteral::printJSON() {
	if(this->value.type == TRUE) {
		return "{\"type\":\"BOOLEAN_LITERAL\",\"value\":\"true\"}";
	}
	else {
		return "{\"type\":\"BOOLEAN_LITERAL\",\"value\":\"false\"}";
	}
}

ts::InstructionReturn BooleanLiteral::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::PUSH;
	instruction->push.entry = ts::Entry();
	instruction->push.entry.type = ts::entry::NUMBER;
	instruction->push.entry.setNumber(this->value.type == TRUE);
	return ts::InstructionReturn(instruction, instruction);
}

bool BooleanLiteral::getBoolean() {
	return this->value.type == TRUE;
}