#include "assignStatement.h"

#include "accessStatement.h"
#include "booleanLiteral.h"
#include "../interpreter/interpreter.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "../util/stringToChars.h"

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

ts::instruction::InstructionType AssignStatement::TypeToLocalOperator(TokenType type) {
	switch(type) {
		case PLUS_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_PLUS;
		case MINUS_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_MINUS;
		case SLASH_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_SLASH;
		case ASTERISK_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_ASTERISK;
		case MODULUS_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_MODULUS;
		case OR_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_BITWISE_OR;
		case AND_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_BITWISE_AND;
		case XOR_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_BITWISE_XOR;
		case SHIFT_LEFT_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_SHIFT_LEFT;
		case SHIFT_RIGHT_ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_SHIFT_RIGHT;
		case ASSIGN:
			return ts::instruction::LOCAL_ASSIGN_EQUAL;
		default:
			return ts::instruction::INVALID_INSTRUCTION;
	}
}

ts::instruction::InstructionType AssignStatement::TypeToObjectOperator(TokenType type) {
	switch(type) {
		case PLUS_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_PLUS;
		case MINUS_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_MINUS;
		case SLASH_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_SLASH;
		case ASTERISK_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_ASTERISK;
		case MODULUS_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_MODULUS;
		case OR_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_BITWISE_OR;
		case AND_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_BITWISE_AND;
		case XOR_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_BITWISE_XOR;
		case SHIFT_LEFT_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_SHIFT_LEFT;
		case SHIFT_RIGHT_ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_SHIFT_RIGHT;
		case ASSIGN:
			return ts::instruction::OBJECT_ASSIGN_EQUAL;
		default:
			return ts::instruction::INVALID_INSTRUCTION;
	}
}

ts::InstructionReturn AssignStatement::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	ts::InstructionReturn output;

	AccessStatementCompiled c = this->lvalue->compileAccess(interpreter, scope);
	ts::Instruction* instruction = c.lastAccess;

	// handle object accesses
	if(instruction->type == ts::instruction::OBJECT_ACCESS) {
		instruction->type = AssignStatement::TypeToObjectOperator(this->assignmentToken.type);
		instruction->objectAssign.entry = ts::Entry(); // initialize memory to avoid crash

		new((void*)&instruction->objectAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
		instruction->objectAssign.hash = hash<string>{}(instruction->localAccess.source);
		instruction->objectAssign.dimensions = instruction->localAccess.dimensions;
		instruction->objectAssign.fromStack = false;
		instruction->objectAssign.pushResult = this->parent->shouldPushToStack(this);
		instruction->objectAssign.popObject = true;
	}
	else { // copy access instruction to assign instruction
		instruction->type = AssignStatement::TypeToLocalOperator(this->assignmentToken.type);
		instruction->localAssign.entry = ts::Entry(); // initialize memory to avoid crash
		
		new((void*)&instruction->localAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
		instruction->localAssign.hash = hash<string>{}(instruction->localAccess.source);
		instruction->localAssign.dimensions = instruction->localAccess.dimensions;
		instruction->localAssign.stackIndex = instruction->localAccess.stackIndex;
		instruction->localAssign.fromStack = false;
		instruction->localAssign.pushResult = this->parent->shouldPushToStack(this);
	}

	if(this->rvalue->getType() == NUMBER_LITERAL) {
		instruction->localAssign.entry.setNumber(((NumberLiteral*)this->rvalue)->getNumber());
	}
	else if(this->rvalue->getType() == BOOLEAN_LITERAL) {
		instruction->localAssign.entry.setNumber(((BooleanLiteral*)this->rvalue)->getBoolean());
	}
	else if(this->rvalue->getType() == STRING_LITERAL) {
		string literal = ((StringLiteral*)this->rvalue)->getString();
		instruction->localAssign.entry = ts::Entry();
		instruction->localAssign.entry.setString(stringToChars(literal));
	}
	else if(
		this->rvalue->getType() == MATH_EXPRESSION
		|| this->rvalue->getType() == ACCESS_STATEMENT
		|| this->rvalue->getType() == ASSIGN_STATEMENT
		|| this->rvalue->getType() == NEW_STATEMENT
		|| this->rvalue->getType() == PARENT_STATEMENT
	) {
		output.add(this->rvalue->compile(interpreter, scope));
		instruction->localAssign.fromStack = true;
	}

	output.add(c.output);

	return output;
}
