#include "numberLiteral.h"
#include "../interpreter/interpreter.h"

using namespace ts;

bool NumberLiteral::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == NUMBER;
}

NumberLiteral* NumberLiteral::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NumberLiteral* output = new NumberLiteral(parser);
	output->parent = parent;
	output->number = tokenizer->getToken().lexeme;
	return output;
}

string NumberLiteral::print() {
	return this->number;
}

string NumberLiteral::printJSON() {
	return "{\"type\":\"NUMBER_LITERAL\",\"value\":\"" + this->number + "\"}";
}

InstructionReturn NumberLiteral::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	Instruction* instruction = new Instruction();
	instruction->type = instruction::PUSH;
	instruction->push.entry = ts::Entry();
	instruction->push.entry.type = entry::NUMBER;
	instruction->push.entry.setNumber(stod(this->number));
	return InstructionReturn(instruction, instruction);
}

double NumberLiteral::getNumber() {
	return stod(this->number);
}