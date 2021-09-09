#include "stringLiteral.h"
#include "../interpreter/interpreter.h"

#include "../util/collapseEscape.h"
#include "../util/stringToChars.h"

bool StringLiteral::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == STRING || engine->tokenizer->peekToken().type == TAGGED_STRING;
}

StringLiteral* StringLiteral::Parse(Component* parent, ts::Engine* engine) {
	StringLiteral* output = new StringLiteral(engine);
	output->parent = parent;
	output->value = engine->tokenizer->getToken();

	// handle escape sequences
	try {
		output->valueString = collapseEscape(output->value.lexeme, true);
	}
	catch(const exception &error) {
		engine->parser->error(error.what());
	}

	return output;
}

string StringLiteral::print() {
	if(this->value.type == STRING) {
		return "\"" + this->valueString + "\"";
	}
	else {
		return "'" + this->valueString + "'";
	}
}

string StringLiteral::printJSON() {
	return "{\"type\":\"STRING_LITERAL\",\"value\":\"" + this->valueString + "\",\"isTagged\":" + (this->value.type == STRING ? "false" : "true") + "}";
}

ts::InstructionReturn StringLiteral::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::PUSH;
	instruction->push.entry = ts::Entry();
	instruction->push.entry.type = ts::entry::STRING;
	instruction->push.entry.setString(this->valueString);
	return ts::InstructionReturn(instruction, instruction);
}

string StringLiteral::getString() {
	return this->valueString;
}
