#include "postfixStatement.h"
#include "../interpreter/interpreter.h"

#include "../util/allocateString.h"

bool PostfixStatement::ShouldParse(ts::Engine* engine) {
	Token token = engine->tokenizer->peekToken();
	return token.type == INCREMENT
		|| token.type == DECREMENT;
}

PostfixStatement* PostfixStatement::Parse(AccessStatement* lvalue, Component* parent, ts::Engine* engine) {
	PostfixStatement* output = new PostfixStatement(engine);
	output->parent = parent;
	output->lvalue = lvalue;
	lvalue->setParent(output);
	output->op = engine->parser->expectToken(INCREMENT, DECREMENT);
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

ts::InstructionReturn PostfixStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	AccessStatementCompiled compiled = this->lvalue->compileAccess(engine, context);

	ts::Instruction* instruction = compiled.lastAccess;

	if(this->op.type == INCREMENT) {
		if(instruction->type == ts::instruction::OBJECT_ACCESS) {
			instruction->type = ts::instruction::OBJECT_ASSIGN_INCREMENT;
		}
		else if(instruction->type == ts::instruction::GLOBAL_ACCESS) {
			instruction->type = ts::instruction::GLOBAL_ASSIGN_INCREMENT;
		}
		else if(instruction->type == ts::instruction::LOCAL_ACCESS) {
			instruction->type = ts::instruction::LOCAL_ASSIGN_INCREMENT;
		}
	}
	else {
		if(instruction->type == ts::instruction::OBJECT_ACCESS) {
			instruction->type = ts::instruction::OBJECT_ASSIGN_DECREMENT;
		}
		else if(instruction->type == ts::instruction::GLOBAL_ACCESS) {
			instruction->type = ts::instruction::GLOBAL_ASSIGN_DECREMENT;
		}
		else if(instruction->type == ts::instruction::LOCAL_ACCESS) {
			instruction->type = ts::instruction::LOCAL_ASSIGN_DECREMENT;
		}
	}

	instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash

	// copy access instruction to assign instruction
	instruction->localAssign.dimensions = instruction->localAccess.dimensions;
	instruction->localAssign.fromStack = false;
	instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);

	if(instruction->localAccess.dimensions > 0) {
		instruction->localAssign.stackIndex = -1;
	}
	else {
		instruction->localAssign.stackIndex = context.scope->allocateVariable(instruction->localAssign.destination).stackIndex;
	}
	return compiled.output;
}
