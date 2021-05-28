#include "postfixStatement.h"
#include "../interpreter/interpreter.h"

bool PostfixStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	Token token = tokenizer->peekToken();
	return token.type == INCREMENT
		|| token.type == DECREMENT;
}

PostfixStatement* PostfixStatement::Parse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	PostfixStatement* output = new PostfixStatement(parser);
	output->parent = parent;
	output->lvalue = lvalue;
	lvalue->setParent(output);
	output->op = parser->expectToken(INCREMENT, DECREMENT);
	return output;
}

string PostfixStatement::print() {
	string output = this->lvalue->print() + this->op.lexeme;
	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}

ts::InstructionReturn PostfixStatement::compile(ts::Interpreter* interpreter) {
	AccessStatementCompiled compiled = this->lvalue->compileAccess(interpreter);

	ts::Instruction* instruction = compiled.lastAccess;
	instruction->type = ts::instruction::LOCAL_ASSIGN;
	instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash

	// copy access instruction to assign instruction
	new((void*)&instruction->localAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
	instruction->localAssign.hash = hash<string>{}(instruction->localAccess.source);
	instruction->localAssign.dimensions = instruction->localAccess.dimensions;
	instruction->localAssign.fromStack = false;
	instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);

	if(this->op.type == INCREMENT) {
		instruction->localAssign.operation = ts::instruction::INCREMENT;
	}
	else {
		instruction->localAssign.operation = ts::instruction::DECREMENT;
	}
	return compiled.output;
}
