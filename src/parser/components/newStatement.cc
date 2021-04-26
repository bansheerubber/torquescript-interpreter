#include "newStatement.h"

bool NewStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == NEW;
}

NewStatement* NewStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NewStatement* output = new NewStatement();
	output->parent = parent;

	parser->expectToken(NEW);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid new object name");
	}
	output->className = Symbol::Parse(output, tokenizer, parser);

	// parse call list
	if(!CallStatement::ShouldParse(tokenizer, parser)) {
		parser->error("invalid new object argument list");
	}
	output->arguments = CallStatement::Parse(parent, tokenizer, parser);

	// expect semicolon if we got no arguments in the body of the new object statement
	if(tokenizer->peekToken().type == SEMICOLON) {
		return output; // don't absorb semicolon, a different part of code will do that
	}
	else if(tokenizer->peekToken().type != LEFT_BRACKET) {
		parser->error("expected bracket for property list for new object");
	}

	parser->expectToken(LEFT_BRACKET);

	// parse property declaration
	AccessStatement::DatablockAsSymbol = true;
	while(!tokenizer->eof()) {
		if(Component::ShouldParse(output, tokenizer, parser)) {
			Component* child = Component::Parse(output, tokenizer, parser);
			output->children.push_back(child);

			if(child->getType() != ASSIGN_STATEMENT) {
				parser->error("only expect property assignments in new object, not '%s'", child->print().c_str());
			}

			AccessStatement* lvalue = ((AssignStatement*)child)->getLvalue();
			if(
				lvalue->hasChain()
				|| lvalue->hasCall()
				|| lvalue->chainSize() > 2
				|| lvalue->isLocalVariable()
				|| lvalue->isGlobalVariable()
			) {
				parser->error("did not expect complex property assignment '%s' in new object", child->print().c_str());
			}

			parser->expectToken(SEMICOLON);
		}
		else {
			break;
		}
	}
	AccessStatement::DatablockAsSymbol = false;

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string NewStatement::print() {
	string output = "new " + this->className->print() + this->arguments->print();
	if(this->children.size() != 0) {
		output += " {\n";
		output += this->printBody();
		output += "}";
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}