#include "elseBody.h"

bool ElseBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == ELSE;
}

ElseBody* ElseBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	ElseBody* output = new ElseBody();
	output->parent = body;

	// check if last statement is valid for else
	Component* lastChild = body->getLastChild();
	if(lastChild == nullptr || (lastChild->getType() != IF_STATEMENT && lastChild->getType() != ELSE_IF_STATEMENT)) {
		parser->error("'else' statements must follow 'if' or 'else if' statements");
	}
	
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