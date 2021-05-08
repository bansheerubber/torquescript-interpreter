#include "stringLiteral.h"

bool StringLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == STRING || tokenizer->peekToken().type == TAGGED_STRING;
}

StringLiteral* StringLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	StringLiteral* output = new StringLiteral(parser);
	output->parent = parent;
	output->value = tokenizer->getToken();
	return output;
}

string StringLiteral::print() {
	if(this->value.type == STRING) {
		return "\"" + this->value.lexeme + "\"";
	}
	else {
		return "'" + this->value.lexeme + "'";
	}
}

ts::InstructionReturn StringLiteral::compile() {
	ts::InstructionReturn output;
	if(this->value.type == STRING) {
		ts::Instruction* newFrame = new ts::Instruction();
		newFrame->type = ts::instruction::NEW_FRAME;
		output.first = newFrame;
		output.last = newFrame;
	}
	else {
		ts::Instruction* deleteFrame = new ts::Instruction();
		deleteFrame->type = ts::instruction::DELETE_FRAME;
		deleteFrame->deleteFrame.save = 1;
		output.first = deleteFrame;
		output.last = deleteFrame;
	}
	return output;
}

string StringLiteral::getString() {
	return this->value.lexeme;
}