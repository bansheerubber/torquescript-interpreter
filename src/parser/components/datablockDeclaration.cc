#include "datablockDeclaration.h"

bool DatablockDeclaration::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == DATABLOCK;
}

DatablockDeclaration* DatablockDeclaration::Parse(Body* parent, Tokenizer* tokenizer, Parser* parser) {
	DatablockDeclaration* output = new DatablockDeclaration();
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
		if(Component::ShouldParse(output, tokenizer, parser)) {
			Component* child = Component::Parse(output, tokenizer, parser);
			output->children.push_back(child);

			if(child->getType() != ASSIGN_STATEMENT) {
				parser->error("only expect assignments in datablock, not '%s'", child->print().c_str());
			}

			AccessStatement* lvalue = ((AssignStatement*)child)->getLvalue();
			if(
				lvalue->hasChain()
				|| lvalue->hasCall()
				|| lvalue->chainSize() > 2
				|| lvalue->isLocalVariable()
				|| lvalue->isGlobalVariable()
			) {
				parser->error("did not expect complex assignment '%s' in datablock", child->print().c_str());
			}

			parser->expectToken(SEMICOLON);
		}
		else {
			break;
		}
	}

	parser->expectToken(RIGHT_BRACKET);
	parser->expectToken(SEMICOLON);

	return output;
}

string DatablockDeclaration::print() {
	string output = "datablock " + this->typeName->print() + this->className->print() + " {\n";
	output += this->printBody();
	output += "};\n";
	return output;
}