#include "forBody.h"

bool ForBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == FOR;
}

ForBody* ForBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	ForBody* output = new ForBody(parser);
	output->parent = body;
	
	parser->expectToken(FOR);
	parser->expectToken(LEFT_PARENTHESIS);

	// parse initialization
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' initialization");
	}
	output->initialization = Component::Parse(output, tokenizer, parser);

	parser->expectToken(SEMICOLON);

	// parse conditional
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' conditional");
	}
	output->conditional = Component::Parse(output, tokenizer, parser);

	parser->expectToken(SEMICOLON);

	// parse increment
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' increment");
	}
	output->increment = Component::Parse(output, tokenizer, parser);

	parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line for statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the for statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ForBody::print() {
	string output = "for(" + this->initialization->print() + ";" + this->parser->space + this->conditional->print() + ";" + this->parser->space + this->increment->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}