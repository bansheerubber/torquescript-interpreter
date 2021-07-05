#include "stringLiteral.h"
#include "../interpreter/interpreter.h"

#include "../util/stringToChars.h"

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

string StringLiteral::printJSON() {
	return "{\"type\":\"STRING_LITERAL\",\"value\":\"" + this->value.lexeme + "\",\"isTagged\":" + (this->value.type == STRING ? "false" : "true") + "}";
}

ts::InstructionReturn StringLiteral::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::PUSH;
	instruction->push.entry = ts::Entry();
	instruction->push.entry.type = ts::entry::STRING;
	instruction->push.entry.setString(stringToChars(this->value.lexeme));
	return ts::InstructionReturn(instruction, instruction);
}

string StringLiteral::getString() {
	return this->value.lexeme;
}