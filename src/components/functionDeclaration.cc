#include "functionDeclaration.h"
#include "../interpreter/interpreter.h"

bool FunctionDeclaration::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == FUNCTION;
}

FunctionDeclaration* FunctionDeclaration::Parse(Body* parent, Tokenizer* tokenizer, Parser* parser) {
	if(parent->getType() != SOURCE_FILE && parent->getType() != PACKAGE_DECLARATION) {
		parser->error("cannot declare scoped functions");
	}
	
	FunctionDeclaration* output = new FunctionDeclaration(parser);
	output->parent = parent;
	parser->expectToken(FUNCTION);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid function name");
	}
	output->name1 = Symbol::Parse(output, tokenizer, parser);

	if(tokenizer->peekToken().type == NAMESPACE) {
		parser->expectToken(NAMESPACE);

		// parse a symbol
		if(!Symbol::ShouldParse(tokenizer, parser)) {
			parser->error("invalid function name");
		}
		output->name2 = Symbol::Parse(output, tokenizer, parser);
	}

	// parse the argument list
	if(!CallStatement::ShouldParse(tokenizer, parser)) {
		parser->error("expected arguments for function");
	}
	output->args = CallStatement::Parse(output, tokenizer, parser);

	// find errors in args
	auto it = output->args->getElements();
	for (; it.first != it.second; it.first++) {
		Component* component = (*(it.first)).component;
		if(
			component != nullptr
			&& (
				component->getType() != ACCESS_STATEMENT
				|| !((AccessStatement*)component)->isLocalVariable()
				|| ((AccessStatement*)component)->chainSize() != 1
			)
		) {
			parser->error("unexpected argument '%s' for function declaration", component->print().c_str());
		}
	}

	parser->expectToken(LEFT_BRACKET);
	
	Component::ParseBody(output, tokenizer, parser);

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string FunctionDeclaration::print() {
	string output = "function " + this->name1->print() + this->args->print() + this->parser->space + "{" + this->parser->newLine;
	if(this->name2 != nullptr) {
		output = "function " + this->name1->print() + "::" + this->name2->print() + this->args->print() + this->parser->space + "{" + this->parser->newLine;
	}

	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

ts::InstructionReturn FunctionDeclaration::compile(ts::Interpreter* interpreter) {
	return {};
}
