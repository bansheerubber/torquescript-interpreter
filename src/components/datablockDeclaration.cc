#include "datablockDeclaration.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "assignStatement.h"
#include "callStatement.h"
#include "symbol.h"

bool DatablockDeclaration::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == DATABLOCK;
}

DatablockDeclaration* DatablockDeclaration::Parse(Body* parent, ts::Engine* engine) {
	DatablockDeclaration* output = new DatablockDeclaration(engine);
	output->parent = parent;
	engine->parser->expectToken(DATABLOCK);

	// parse a symbol
	if(!Symbol::ShouldParse(engine)) {
		engine->parser->error("invalid datablock name");
	}
	output->typeName = Symbol::Parse(output, engine);

	// parse inheritance statement
	if(CallStatement::ShouldParse(engine)) {
		output->className = CallStatement::Parse(output, engine);
	}
	else {
		engine->parser->error("got invalid name for datablock");
	}

	if(output->className->getElementCount() == 0) {
		engine->parser->error("got no name for datablock");
	}

	// make sure we got a valid name for the datablock
	Component* firstComponent = output->className->getElement(0).component;
	if(
		firstComponent->getType() != INHERITANCE_STATEMENT
		&& firstComponent->getType() != STRING_LITERAL
		&& firstComponent->getType() != SYMBOL_STATEMENT
		&& firstComponent->getType() != ACCESS_STATEMENT
		&& firstComponent->getType() != MATH_EXPRESSION
	) {
		engine->parser->error("got invalid name for datablock");
	}
	
	engine->parser->expectToken(LEFT_BRACKET);
	// read assignment statements
	while(!engine->tokenizer->eof()) {
		if(
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
				engine->parser->error("did not expect complex property assignment '%s' in datablock", access->print().c_str());
			}

			// now parse the assign statement
			if(!AssignStatement::ShouldParse(access, output, engine)) {
				engine->parser->error("expected property assignment in datablock");
			}

			output->children.push_back(AssignStatement::Parse(access, output, engine));

			engine->parser->expectToken(SEMICOLON);
		}
		else if(engine->tokenizer->peekToken().type == RIGHT_BRACKET) {
			break;
		}
		else {
			engine->parser->error("expected property assignment in datablock");
		}
	}

	engine->parser->expectToken(RIGHT_BRACKET);
	engine->parser->expectToken(SEMICOLON);

	return output;
}

string DatablockDeclaration::print() {
	string output = "datablock " + this->typeName->print() + this->className->print() + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "};" + this->engine->parser->newLine;
	return output;
}

string DatablockDeclaration::printJSON() {
	return "{\"type\":\"DATABLOCK_DECLARATION\",\"typeName\":" + this->typeName->printJSON() + ",\"className\":" + this->className->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn DatablockDeclaration::compile(ts::Engine* engine, ts::CompilationContext context) {
	this->engine->parser->error("%s not supported", this->engine->parser->typeToName(this->getType()));
	return {};
}
