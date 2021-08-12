#include "datablockDeclaration.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "assignStatement.h"
#include "callStatement.h"
#include "symbol.h"

bool DatablockDeclaration::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == DATABLOCK;
}

DatablockDeclaration* DatablockDeclaration::Parse(Body* parent, Tokenizer* tokenizer, Parser* parser) {
	DatablockDeclaration* output = new DatablockDeclaration(parser);
	output->parent = parent;
	parser->expectToken(DATABLOCK);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid datablock name");
	}
	output->typeName = Symbol::Parse(output, tokenizer, parser);

	// parse inheritance statement
	if(CallStatement::ShouldParse(tokenizer, parser)) {
		output->className = CallStatement::Parse(output, tokenizer, parser);
	}
	else {
		parser->error("got invalid name for datablock");
	}

	if(output->className->getElementCount() == 0) {
		parser->error("got no name for datablock");
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
		parser->error("got invalid name for datablock");
	}
	
	parser->expectToken(LEFT_BRACKET);
	// read assignment statements
	while(!tokenizer->eof()) {
		if(
			Symbol::ShouldParse(tokenizer, parser)
			|| Symbol::ShouldParseAlphabeticToken(tokenizer, parser)
		) {
			Symbol* symbol = Symbol::Parse(output, tokenizer, parser);
			AccessStatement* access = AccessStatement::Parse(symbol, output, tokenizer, parser, true);
			symbol->parent = access;
			if(
				access->hasChain()
				|| access->hasCall()
				|| access->chainSize() > 2
				|| access->isLocalVariable()
				|| access->isGlobalVariable()
			) {
				parser->error("did not expect complex property assignment '%s' in datablock", access->print().c_str());
			}

			// now parse the assign statement
			if(!AssignStatement::ShouldParse(access, output, tokenizer, parser)) {
				parser->error("expected property assignment in datablock");
			}

			output->children.push_back(AssignStatement::Parse(access, output, tokenizer, parser));

			parser->expectToken(SEMICOLON);
		}
		else if(tokenizer->peekToken().type == RIGHT_BRACKET) {
			break;
		}
		else {
			parser->error("expected property assignment in datablock");
		}
	}

	parser->expectToken(RIGHT_BRACKET);
	parser->expectToken(SEMICOLON);

	return output;
}

string DatablockDeclaration::print() {
	string output = "datablock " + this->typeName->print() + this->className->print() + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "};" + this->parser->newLine;
	return output;
}

string DatablockDeclaration::printJSON() {
	return "{\"type\":\"DATABLOCK_DECLARATION\",\"typeName\":" + this->typeName->printJSON() + ",\"className\":" + this->className->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn DatablockDeclaration::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	this->parser->error("%s not supported", this->parser->typeToName(this->getType()));
	return {};
}