#include "postfixStatement.h"
#include "../interpreter/interpreter.h"

#include "../util/allocateString.h"

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

ts::InstructionReturn PostfixStatement::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	AccessStatementCompiled compiled = this->lvalue->compileAccess(interpreter, context);

	ts::Instruction* instruction = compiled.lastAccess;

	if(this->op.type == INCREMENT) {
		instruction->type = ts::instruction::LOCAL_ASSIGN_INCREMENT;
	}
	else {
		instruction->type = ts::instruction::LOCAL_ASSIGN_DECREMENT;
	}

	instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash

	// copy access instruction to assign instruction
	ALLOCATE_STRING(instruction->localAccess.source, instruction->localAssign.destination);
	instruction->localAssign.hash = hash<string>{}(instruction->localAccess.source);
	instruction->localAssign.dimensions = instruction->localAccess.dimensions;
	instruction->localAssign.fromStack = false;
	instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);
	instruction->localAssign.stackIndex = context.scope->allocateVariable(instruction->localAssign.destination).stackIndex;
	return compiled.output;
}
