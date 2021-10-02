#include "booleanLiteral.h"
#include "../interpreter/interpreter.h"

bool BooleanLiteral::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == TRUE || engine->tokenizer->peekToken().type == FALSE;
}

BooleanLiteral* BooleanLiteral::Parse(Component* parent, ts::Engine* engine) {
	BooleanLiteral* output = new BooleanLiteral(engine);
	output->parent = parent;
	output->value = engine->tokenizer->getToken();
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

ts::InstructionReturn BooleanLiteral::compile(ts::Engine* engine, ts::CompilationContext context) {
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
