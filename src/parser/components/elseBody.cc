#include "elseBody.h"

bool ElseBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == ELSE;
}

ElseBody* ElseBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	ElseBody* output = new ElseBody(parser);
	output->parent = body;

	parser->expectToken(ELSE);

	// handle one line else statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the else statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ElseBody::print() {
	string output = "else {\n";
	output += this->printBody();
	output += "}\n";
	return output;
}