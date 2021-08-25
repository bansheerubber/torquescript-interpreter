#include "stringLiteral.h"
#include "../interpreter/interpreter.h"

#include "../util/stringToChars.h"

bool StringLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == STRING || tokenizer->peekToken().type == TAGGED_STRING;
}

#define INCREMENT_ESCAPE_SEQUENCE() i++;\
	if(i >= value.length()) {\
		parser->error("incomplete escape sequence in string literal");\
		break;\
	}

StringLiteral* StringLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	StringLiteral* output = new StringLiteral(parser);
	output->parent = parent;
	output->value = tokenizer->getToken();

	// handle escape sequences
	string value = output->value.lexeme;
	string valueOutput;
	for(size_t i = 0; i < value.length(); i++) {
		if(value[i] == '\\') {
			INCREMENT_ESCAPE_SEQUENCE();
			char command = value[i];
			switch(command) {
				case 'x': {
					INCREMENT_ESCAPE_SEQUENCE();
					char hex0 = tolower(value[i]);
					int digit0 = hex0 >= 97
						? hex0 - 97 + 10
						: hex0 - 48;

					INCREMENT_ESCAPE_SEQUENCE();
					char hex1 = tolower(value[i]);
					int digit1 = hex1 >= 97
						? hex1 - 97 + 10
						: hex1 - 48;
					
					if(
						(
							digit0 < 0
							|| digit0 > 15
						)
						|| (
							digit1 < 0
							|| digit1 > 15
						)
					) {
						parser->error("invalid escape sequence in string literal");
					}

					char combined = 16 * digit0 + digit1;
					valueOutput += combined;
					break;
				}

				case 'n': {
					valueOutput += '\n';
					break;
				}

				case 'r': {
					valueOutput += '\r';
					break;
				}

				case 't': {
					valueOutput += '\t';
					break;
				}

				default: {
					// parser->warning("invalid escape sequence in string literal");
					valueOutput += command;
				}
			}
		}
		else {
			valueOutput += value[i];
		}
	}

	output->valueString = valueOutput;

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

ts::InstructionReturn StringLiteral::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::PUSH;
	instruction->push.entry = ts::Entry();
	instruction->push.entry.type = ts::entry::STRING;
	instruction->push.entry.setString(stringToChars(this->valueString));
	return ts::InstructionReturn(instruction, instruction);
}

string StringLiteral::getString() {
	return this->valueString;
}