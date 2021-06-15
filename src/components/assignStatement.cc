#include "assignStatement.h"
#include "accessStatement.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "mathExpression.h"
#include "../interpreter/interpreter.h"

bool AssignStatement::ShouldParse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	Token token = tokenizer->peekToken();
	return (
		token.type == PLUS_ASSIGN
		|| token.type == MINUS_ASSIGN
		|| token.type == SLASH_ASSIGN
		|| token.type == ASTERISK_ASSIGN
		|| token.type == OR_ASSIGN
		|| token.type == AND_ASSIGN
		|| token.type == XOR_ASSIGN
		|| token.type == SHIFT_LEFT_ASSIGN
		|| token.type == SHIFT_RIGHT_ASSIGN
		|| token.type == MODULUS_ASSIGN
		|| token.type == ASSIGN
	) && (
		parent->getType() == DATABLOCK_DECLARATION
		|| parent->getType() == NEW_STATEMENT
		|| (
			(lvalue != nullptr && lvalue->IsValidLvalue()) || lvalue == nullptr
		)
	);
}

AssignStatement* AssignStatement::Parse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	AssignStatement* output = new AssignStatement(parser);
	output->parent = parent;
	lvalue->setParent(output);
	output->lvalue = lvalue;
	output->assignmentToken = tokenizer->getToken();

	// parse access statement first since we need to absorb it before checking for math
	if(Component::ShouldParse(output, tokenizer, parser)) {
		output->rvalue = Component::Parse(output, tokenizer, parser);
	}
	else {
		parser->error("could not find valid rvalue for assign");
	}

	return output;
}

string AssignStatement::print() {
	string output = this->lvalue->print() + this->parser->space + this->assignmentToken.lexeme + this->parser->space + this->rvalue->print();
	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}

string AssignStatement::printJSON() {
	return "{\"type\":\"ASSIGN_STATEMENT\",\"assignmentToken\":\"" + this->assignmentToken.lexeme + "\",\"lvalue\":" + this->lvalue->printJSON() + ",\"rvalue\":" + this->rvalue->printJSON() + "}";
}

ts::instruction::AssignOperations AssignStatement::TypeToOperator(TokenType type) {
	switch(type) {
		case PLUS_ASSIGN:
			return ts::instruction::PLUS_EQUALS;
		case MINUS_ASSIGN:
			return ts::instruction::MINUS_EQUALS;
		case SLASH_ASSIGN:
			return ts::instruction::SLASH_EQUALS;
		case ASTERISK_ASSIGN:
			return ts::instruction::ASTERISK_EQUALS;
		case MODULUS_ASSIGN:
			return ts::instruction::MODULUS_EQUALS;
		case OR_ASSIGN:
			return ts::instruction::BITWISE_OR_EQUALS;
		case AND_ASSIGN:
			return ts::instruction::BITWISE_AND_EQUALS;
		case XOR_ASSIGN:
			return ts::instruction::BITWISE_XOR_EQUALS;
		case SHIFT_LEFT_ASSIGN:
			return ts::instruction::SHIFT_LEFT_EQUALS;
		case SHIFT_RIGHT_ASSIGN:
			return ts::instruction::SHIFT_RIGHT_EQUALS;
		case ASSIGN:
			return ts::instruction::EQUALS;
		default:
			return ts::instruction::INVALID_ASSIGN;
	}
}

ts::InstructionReturn AssignStatement::compile(ts::Interpreter* interpreter) {
	ts::InstructionReturn output;

	AccessStatementCompiled c = this->lvalue->compileAccess(interpreter);
	ts::Instruction* instruction = c.lastAccess;
	instruction->type = ts::instruction::LOCAL_ASSIGN;
	instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash

	// copy access instruction to assign instruction
	new((void*)&instruction->localAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
	instruction->localAssign.hash = hash<string>{}(instruction->localAccess.source);
	instruction->localAssign.dimensions = instruction->localAccess.dimensions;
	instruction->localAssign.fromStack = false;
	instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);
	instruction->localAssign.operation = AssignStatement::TypeToOperator(this->assignmentToken.type);

	if(this->rvalue->getType() == NUMBER_LITERAL) {
		instruction->localAssign.entry.setNumber(((NumberLiteral*)this->rvalue)->getNumber());
	}
	else if(this->rvalue->getType() == STRING_LITERAL) {
		string literal = ((StringLiteral*)this->rvalue)->getString();
		instruction->localAssign.entry.setString(literal);
	}
	else if(
		this->rvalue->getType() == MATH_EXPRESSION
		|| this->rvalue->getType() == ACCESS_STATEMENT
		|| this->rvalue->getType() == ASSIGN_STATEMENT
	) {
		output.add(this->rvalue->compile(interpreter));
		instruction->localAssign.fromStack = true;
	}

	output.add(c.output);

	return output;
}
