#include "newStatement.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "assignStatement.h"
#include "booleanLiteral.h"
#include "callStatement.h"
#include "inheritanceStatement.h"
#include "mathExpression.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "../util/stringToChars.h"
#include "symbol.h"

bool NewStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		tokenizer->peekToken().type == NEW
		&& (
			tokenizer->peekToken(1).type == SYMBOL
			|| tokenizer->peekToken(1).type == LEFT_PARENTHESIS
		)
	);
}

NewStatement* NewStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NewStatement* output = new NewStatement(parser);
	output->parent = parent;

	parser->expectToken(NEW);

	// parse a symbol
	if(Symbol::ShouldParse(tokenizer, parser)) {
		output->className = Symbol::Parse(output, tokenizer, parser);
	}
	else if(MathExpression::ShouldParse(nullptr, tokenizer, parser)) {
		output->className = MathExpression::Parse(nullptr, output, tokenizer, parser);
	}
	else {
		parser->error("invalid new object name");
	}

	// parse call list
	if(InheritanceStatement::ShouldParse(tokenizer, parser)) {
		output->arguments = InheritanceStatement::Parse(parent, tokenizer, parser);
	}
	else if(CallStatement::ShouldParse(tokenizer, parser)) {
		output->arguments = CallStatement::Parse(parent, tokenizer, parser);
	}
	else {
		parser->error("invalid new object argument list");
	}

	// expect something not a left bracket if we got no arguments in the body of the new object statement
	if(tokenizer->peekToken().type != LEFT_BRACKET) {
		return output;
	}

	parser->expectToken(LEFT_BRACKET);

	// parse property declaration
	while(!tokenizer->eof()) {
		// new statements can be nested, apparently
		if(NewStatement::ShouldParse(tokenizer, parser)) {
			output->children.push_back(NewStatement::Parse(output, tokenizer, parser));
			parser->expectToken(SEMICOLON);
		}
		else if(AccessStatement::ShouldParse(tokenizer, parser, true)) {
			AccessStatement* access = AccessStatement::Parse(nullptr, output, tokenizer, parser, true);
			if(
				access->hasChain()
				|| access->hasCall()
				|| access->chainSize() > 2
				|| access->isLocalVariable()
				|| access->isGlobalVariable()
			) {
				parser->error("did not expect complex property assignment '%s' in new object", access->print().c_str());
			}

			// now parse the assign statement
			if(!AssignStatement::ShouldParse(access, output, tokenizer, parser)) {
				parser->error("expected property assignment in new object");
			}
			output->children.push_back(AssignStatement::Parse(access, output, tokenizer, parser));

			parser->expectToken(SEMICOLON);
		}
		else if(tokenizer->peekToken().type == RIGHT_BRACKET) {
			break;
		}
		else {
			parser->error("expected property assignment in new object");
		}
	}

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string NewStatement::print() {
	string output = "new " + this->className->print() + this->arguments->print();
	if(this->children.size() != 0) {
		output += this->parser->space + "{" + this->parser->newLine;
		output += this->printBody();
		output += "}";
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}

string NewStatement::printJSON() {
	if(this->children.size() == 0) {
		return "{\"type\":\"NEW_STATEMENT\",\"className\":" + this->className->printJSON() + ",\"arguments\":" + this->arguments->printJSON() + "}";
	}
	else {
		return "{\"type\":\"NEW_STATEMENT\",\"className\":" + this->className->printJSON() + ",\"arguments\":" + this->arguments->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
	}
}

ts::InstructionReturn NewStatement::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;
	
	ts::Instruction* createObject = new ts::Instruction();
	createObject->type = ts::instruction::CREATE_OBJECT;
	new((void*)&createObject->createObject.type) string(this->className->print()); // TODO move this initialization elsewhere
	createObject->createObject.namespaceIndex = 0;
	createObject->createObject.isCached = false;
	output.add(createObject);

	for(Component* component: this->children) {
		if(component->getType() == ASSIGN_STATEMENT) {
			AssignStatement* assignStatement = (AssignStatement*)component;

			AccessStatementCompiled c = assignStatement->getLValue()->compileAccess(interpreter, context);
			ts::Instruction* instruction = c.lastAccess;
			
			instruction->type = ts::instruction::OBJECT_ASSIGN_EQUAL;
			instruction->objectAssign.entry = ts::Entry(); // initialize memory to avoid crash

			new((void*)&instruction->objectAssign.destination) string(instruction->localAccess.source); // TODO move this initialization elsewhere
			instruction->objectAssign.hash = hash<string>{}(instruction->localAccess.source);
			instruction->objectAssign.dimensions = instruction->localAccess.dimensions;
			instruction->objectAssign.fromStack = false;
			instruction->objectAssign.pushResult = false;
			instruction->objectAssign.popObject = false;

			if(assignStatement->getRValue()->getType() == NUMBER_LITERAL) {
				instruction->objectAssign.entry.setNumber(((NumberLiteral*)assignStatement->getRValue())->getNumber());
			}
			else if(assignStatement->getRValue()->getType() == BOOLEAN_LITERAL) {
				instruction->objectAssign.entry.setNumber(((BooleanLiteral*)assignStatement->getRValue())->getBoolean());
			}
			else if(assignStatement->getRValue()->getType() == STRING_LITERAL) {
				string literal = ((StringLiteral*)assignStatement->getRValue())->getString();
				instruction->objectAssign.entry = ts::Entry();
				instruction->objectAssign.entry.setString(stringToChars(literal));
			}
			else if(
				assignStatement->getRValue()->getType() == MATH_EXPRESSION
				|| assignStatement->getRValue()->getType() == ACCESS_STATEMENT
				|| assignStatement->getRValue()->getType() == ASSIGN_STATEMENT
				|| assignStatement->getRValue()->getType() == NEW_STATEMENT
			) {
				output.add(assignStatement->getRValue()->compile(interpreter, context));
				instruction->objectAssign.fromStack = true;
			}

			output.add(c.output);
		}
	}

	// pop from stack if needed
	if(!this->parent->shouldPushToStack(this)) {
		ts::Instruction* pop = new ts::Instruction();
		pop->type = ts::instruction::POP;
		output.add(pop);
	}

	return output;
}