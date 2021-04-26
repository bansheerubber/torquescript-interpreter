#include "ifBody.h"

bool IfBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == IF;
}

IfBody* IfBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	IfBody* output = new IfBody();
	output->parent = body;
	
	parser->expectToken(IF);
	parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'if' conditional");
	}

	output->conditional = Component::Parse(output, tokenizer, parser);
	parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line if statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the if statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string IfBody::print() {
	string output = "if(" + this->conditional->print() + ") {\n";
	output += this->printBody();
	output += "}\n";
	return output;
}