#include "whileBody.h"

bool WhileBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == WHILE;
}

WhileBody* WhileBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	WhileBody* output = new WhileBody(parser);
	output->parent = body;
	
	parser->expectToken(WHILE);
	parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'while' conditional");
	}

	output->conditional = Component::Parse(output, tokenizer, parser);
	parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line while statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the while statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string WhileBody::print() {
	string output = "while(" + this->conditional->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}