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

string PostfixStatement::printJSON() {
	return "{\"type\":\"POSTFIX_STATEMENT\",\"lvalue\":" + this->lvalue->printJSON() + ",\"operation\":\"" + this->op.lexeme + "\"}";
}

ts::InstructionReturn PostfixStatement::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	AccessStatementCompiled compiled = this->lvalue->compileAccess(interpreter, scope);

	ts::Instruction* instruction = compiled.lastAccess;

	if(this->op.type == INCREMENT) {
		instruction->type = ts::instruction::LOCAL_ASSIGN_INCREMENT;
	}
	else {
		instruction->type = ts::instruction::LOCAL_ASSIGN_DECREMENT;
	}

	instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash

	// copy access instruction to assign instruction
	new((void*)&instruction->localAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
	instruction->localAssign.hash = hash<string>{}(instruction->localAccess.source);
	instruction->localAssign.dimensions = instruction->localAccess.dimensions;
	instruction->localAssign.fromStack = false;
	instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);
	return compiled.output;
}
