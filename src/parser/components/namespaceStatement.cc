#include "namespaceStatement.h"

bool NamespaceStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		tokenizer->peekToken().type == PARENT
		|| (
			tokenizer->peekToken().type == SYMBOL
			&& tokenizer->peekToken(1).type == NAMESPACE
		)
	);
}

NamespaceStatement* NamespaceStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NamespaceStatement* output = new NamespaceStatement(parser);
	output->parent = parent;

	if(tokenizer->peekToken().type == PARENT) {
		parser->expectToken(PARENT);
	}
	else {
		// parse a symbol
		if(!Symbol::ShouldParse(tokenizer, parser)) {
			parser->error("invalid namespace operation");
		}
		output->name = Symbol::Parse(output, tokenizer, parser);	
	}

	parser->expectToken(NAMESPACE);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid namespace operation");
	}
	output->operation = Symbol::Parse(output, tokenizer, parser);

	// parse call statement
	if(!CallStatement::ShouldParse(tokenizer, parser)) {
		parser->error("invalid namespace operation");
	}
	output->call = CallStatement::Parse(parent, tokenizer, parser);

	return output;
}

string NamespaceStatement::print() {
	string output = "Parent::" + this->operation->print() + this->call->print();
	if(this->name != nullptr) {
		output = this->name->print() + "::" + this->operation->print() + this->call->print();
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}

	return output;
}