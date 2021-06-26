#include "datablockDeclaration.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "assignStatement.h"

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
	if(!InheritanceStatement::ShouldParse(tokenizer, parser)) {
		parser->error("invalid datablock class name");
	}
	output->className = InheritanceStatement::Parse(output, tokenizer, parser);
	
	parser->expectToken(LEFT_BRACKET);
	// read assignment statements
	while(!tokenizer->eof()) {
		if(AccessStatement::ShouldParse(tokenizer, parser, true)) {
			AccessStatement* access = AccessStatement::Parse(nullptr, output, tokenizer, parser, true);
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

ts::InstructionReturn DatablockDeclaration::compile(ts::Interpreter* interpreter) {
	this->parser->error("%s not supported", this->parser->typeToName(this->getType()));
	return {};
}