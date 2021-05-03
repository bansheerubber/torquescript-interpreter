#include "defaultBody.h"

bool DefaultBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == DEFAULT;
}

DefaultBody* DefaultBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	DefaultBody* output = new DefaultBody(parser);
	output->parent = body;
	
	parser->expectToken(DEFAULT);
	parser->expectToken(COLON);

	Component::ParseBody(output, tokenizer, parser); // parse the body of the case statement
	
	return output;
}

string DefaultBody::print() {
	string output = "default:" + this->parser->newLine;
	output += this->printBody();
	return output;
}

ts::InstructionReturn DefaultBody::compile() {
	return {};
}
