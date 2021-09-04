#include "newStatement.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "../util/allocateString.h"
#include "assignStatement.h"
#include "booleanLiteral.h"
#include "callStatement.h"
#include "inheritanceStatement.h"
#include "mathExpression.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "../util/stringToChars.h"
#include "symbol.h"

bool NewStatement::ShouldParse(ts::Engine* engine) {
	return (
		engine->tokenizer->peekToken().type == NEW
		&& (
			engine->tokenizer->peekToken(1).type == SYMBOL
			|| engine->tokenizer->peekToken(1).type == LEFT_PARENTHESIS
		)
	);
}

NewStatement* NewStatement::Parse(Component* parent, ts::Engine* engine) {
	NewStatement* output = new NewStatement(engine);
	output->parent = parent;

	engine->parser->expectToken(NEW);

	// parse a symbol
	if(Symbol::ShouldParse(engine)) {
		output->className = Symbol::Parse(output, engine);
	}
	else if(MathExpression::ShouldParse(nullptr, engine)) {
		output->className = MathExpression::Parse(nullptr, output, engine);
	}
	else {
		engine->parser->error("invalid new object name");
	}

	if(CallStatement::ShouldParse(engine)) {
		output->arguments = CallStatement::Parse(output, engine);
	}
	else {
		engine->parser->error("invalid new object argument list");
	}

	if(output->arguments->getElementCount() > 0) {
		// make sure we got a valid name for the object
		Component* firstComponent = output->arguments->getElement(0).component;
		if(
			firstComponent->getType() != INHERITANCE_STATEMENT
			&& firstComponent->getType() != STRING_LITERAL
			&& firstComponent->getType() != SYMBOL_STATEMENT
			&& firstComponent->getType() != ACCESS_STATEMENT
			&& firstComponent->getType() != MATH_EXPRESSION
		) {
			engine->parser->error("got invalid name for new object");
		}
	}

	// expect something not a left bracket if we got no arguments in the body of the new object statement
	if(engine->tokenizer->peekToken().type != LEFT_BRACKET) {
		return output;
	}

	engine->parser->expectToken(LEFT_BRACKET);

	// parse property declaration
	while(!engine->tokenizer->eof()) {
		// new statements can be nested, apparently
		if(NewStatement::ShouldParse(engine)) {
			output->children.push_back(NewStatement::Parse(output, engine));
			engine->parser->expectToken(SEMICOLON);
		}
		else if(
			Symbol::ShouldParse(engine)
			|| Symbol::ShouldParseAlphabeticToken(engine)
		) {
			Symbol* symbol = Symbol::Parse(output, engine);
			AccessStatement* access = AccessStatement::Parse(symbol, output, engine, true);
			symbol->parent = access;
			if(
				access->hasChain()
				|| access->hasCall()
				|| access->chainSize() > 2
				|| access->isLocalVariable()
				|| access->isGlobalVariable()
			) {
				engine->parser->error("did not expect complex property assignment '%s' in new object", access->print().c_str());
			}

			// now parse the assign statement
			if(!AssignStatement::ShouldParse(access, output, engine)) {
				engine->parser->error("expected property assignment in new object");
			}

			AssignStatement* assign = AssignStatement::Parse(access, output, engine);
			output->children.push_back(assign);

			if(access->elements[0].component->getType() == SYMBOL_STATEMENT) {
				string variableName = ((Symbol*)access->elements[0].component)->print();
				if(toLower(variableName) == "class") {
					output->classProperty = assign->getRValue();
				}
				else if(toLower(variableName) == "superclass") {
					output->superClassProperty = assign->getRValue();
				}
			}

			engine->parser->expectToken(SEMICOLON);
		}
		else if(engine->tokenizer->peekToken().type == RIGHT_BRACKET) {
			break;
		}
		else {
			engine->parser->error("expected property assignment in new object");
		}
	}

	engine->parser->expectToken(RIGHT_BRACKET);

	return output;
}

string NewStatement::print() {
	string output = "new " + this->className->print() + this->arguments->print();
	if(this->children.size() != 0) {
		output += this->engine->parser->space + "{" + this->engine->parser->newLine;
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

ts::InstructionReturn NewStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;

	ts::Instruction* createObject = new ts::Instruction();
	createObject->type = ts::instruction::CREATE_OBJECT;
	createObject->createObject.canCreate = true;
	createObject->createObject.symbolNameCached = false;
	ALLOCATE_STRING(string(""), createObject->createObject.inheritedName);

	if(this->className->getType() == SYMBOL_STATEMENT) {
		ALLOCATE_STRING(this->className->print(), createObject->createObject.typeName);
		createObject->createObject.typeNameCached = true;
		createObject->createObject.typeMethodTree = engine->interpreter->getNamespace(createObject->createObject.typeName);
	}
	else {
		ALLOCATE_STRING(string(""), createObject->createObject.typeName);
		createObject->createObject.typeNameCached = false;
		createObject->createObject.typeMethodTree = nullptr;
		
		output.add(this->className->compile(engine, context));
	}

	string symbolName;
	if(this->arguments->getElementCount() == 0) { // no name case
		ALLOCATE_STRING(string(""), createObject->createObject.symbolName);
		createObject->createObject.symbolNameCached = true;
	}
	else {
		Component* firstComponent = this->arguments->getElement(0).component;
		if(firstComponent->getType() == INHERITANCE_STATEMENT) {
			ALLOCATE_STRING(((InheritanceStatement*)firstComponent)->parentClass->print(), createObject->createObject.inheritedName);	
			firstComponent = ((InheritanceStatement*)firstComponent)->className;
		}
		
		if(firstComponent != nullptr) {
			if(firstComponent->getType() == SYMBOL_STATEMENT) { // handle first literal type
				symbolName = ((Symbol*)firstComponent)->print();
				ALLOCATE_STRING(symbolName, createObject->createObject.symbolName);
				createObject->createObject.symbolNameCached = true;
			}
			else if(firstComponent->getType() == STRING_LITERAL) { // handle second literal type
				symbolName = ((StringLiteral*)firstComponent)->getString();
				ALLOCATE_STRING(symbolName, createObject->createObject.symbolName);
				createObject->createObject.symbolNameCached = true;
			}
		}

		if(!createObject->createObject.symbolNameCached) {
			ALLOCATE_STRING(string(""), createObject->createObject.symbolName);

			if(firstComponent) {
				output.add(firstComponent->compile(engine, context));
			}
			else {
				createObject->createObject.symbolNameCached = true;
			}
		}
	}

	string classProperty;
	if(this->classProperty) {
		if(this->classProperty->getType() == SYMBOL_STATEMENT) {
			classProperty = ((Symbol*)this->classProperty)->print();
			ALLOCATE_STRING(classProperty, createObject->createObject.classProperty);
			createObject->createObject.classPropertyCached = true;
		}
		else if(this->classProperty->getType() == STRING_LITERAL) {
			classProperty = ((StringLiteral*)this->classProperty)->getString();
			ALLOCATE_STRING(classProperty, createObject->createObject.classProperty);
			createObject->createObject.classPropertyCached = true;
		}
		else {
			ALLOCATE_STRING(string(""), createObject->createObject.classProperty);
			createObject->createObject.classPropertyCached = false;
			
			output.add(this->classProperty->compile(engine, context));
		}
	}
	else {
		ALLOCATE_STRING(string(""), createObject->createObject.classProperty);
		createObject->createObject.classPropertyCached = true;
	}

	string superClassProperty;
	if(this->superClassProperty) {
		if(this->superClassProperty->getType() == SYMBOL_STATEMENT) {
			superClassProperty = ((Symbol*)this->superClassProperty)->print();
			ALLOCATE_STRING(superClassProperty, createObject->createObject.superClassProperty);
			createObject->createObject.superClassPropertyCached = true;
		}
		else if(this->classProperty->getType() == STRING_LITERAL) {
			superClassProperty = ((StringLiteral*)this->superClassProperty)->getString();
			ALLOCATE_STRING(superClassProperty, createObject->createObject.superClassProperty);
			createObject->createObject.superClassPropertyCached = true;
		}
		else {
			ALLOCATE_STRING(string(""), createObject->createObject.superClassProperty);
			createObject->createObject.superClassPropertyCached = false;
			
			output.add(this->superClassProperty->compile(engine, context));
		}
	}
	else {
		ALLOCATE_STRING(string(""), createObject->createObject.superClassProperty);
		createObject->createObject.superClassPropertyCached = true;
	}

	bool canCacheMethodTree = createObject->createObject.typeNameCached
		&& createObject->createObject.symbolNameCached
		&& createObject->createObject.classPropertyCached
		&& createObject->createObject.superClassPropertyCached;

	if(canCacheMethodTree) {
		ts::MethodTree* typeCheck = engine->interpreter->getNamespace(this->className->print());
		if(typeCheck != nullptr && typeCheck->isTSSL) {
			ts::MethodTree* tree = engine->interpreter->createMethodTreeFromNamespaces(
				symbolName,
				classProperty,
				superClassProperty,
				this->className->print()
			);
			createObject->createObject.methodTree = tree;
			createObject->createObject.isCached = true;
		}
		else {
			createObject->createObject.canCreate = false;
		}
	}
	else {
		createObject->createObject.methodTree = nullptr;
		createObject->createObject.isCached = false;
	}

	output.add(createObject);

	for(Component* component: this->children) {
		if(component->getType() == ASSIGN_STATEMENT) {
			AssignStatement* assignStatement = (AssignStatement*)component;

			AccessStatementCompiled c = assignStatement->getLValue()->compileAccess(engine, context);
			ts::Instruction* instruction = c.lastAccess;
			
			instruction->type = ts::instruction::OBJECT_ASSIGN_EQUAL;
			instruction->objectAssign.entry = ts::Entry(); // initialize memory to avoid crash

			ALLOCATE_STRING(instruction->symbolAccess.source, instruction->objectAssign.destination);
			instruction->objectAssign.hash = hash<string>{}(instruction->symbolAccess.source);
			instruction->objectAssign.dimensions = instruction->symbolAccess.dimensions;
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
				output.add(assignStatement->getRValue()->compile(engine, context));
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
