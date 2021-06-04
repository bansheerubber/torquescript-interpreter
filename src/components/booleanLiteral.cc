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

ts::InstructionReturn BooleanLiteral::compile(ts::Interpreter* interpreter) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::PUSH;
	instruction->push.entry.type = ts::entry::NUMBER;
	instruction->push.entry.setNumber(this->value.type == TRUE);
	return ts::InstructionReturn(instruction, instruction);
}