#include "switchBody.h"
#include "../interpreter/interpreter.h"

bool SwitchBody::ShouldParse(Tokenizer* tokenizer, class Parser* parser) {
	Token token = tokenizer->peekToken();
	return token.type == SWITCH || token.type == STRING_SWITCH;
}

SwitchBody* SwitchBody::Parse(Body* body, Tokenizer* tokenizer, class Parser* parser) {
	SwitchBody* output = new SwitchBody(parser);
	output->parent = body;

	parser->expectToken(SWITCH, STRING_SWITCH);
	parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'switch' conditional");
	}
	output->conditional = Component::Parse(output, tokenizer, parser);
	
	parser->expectToken(RIGHT_PARENTHESIS);
	parser->expectToken(LEFT_BRACKET);

	while(!tokenizer->eof()) {
		Token token = tokenizer->peekToken();
		if(CaseBody::ShouldParse(tokenizer, parser)) {
			output->children.push_back(CaseBody::Parse(output, tokenizer, parser));
		}
		else if(DefaultBody::ShouldParse(tokenizer, parser)) {
			output->children.push_back(DefaultBody::Parse(output, tokenizer, parser));
		}
		else if(token.type == RIGHT_BRACKET) {
			break;
		}
		else {
			parser->error("unexpected token '%s' in switch statement body", token.lexeme.c_str());
		}
	}

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string SwitchBody::print() {
	string output = "switch(" + this->conditional->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

ts::InstructionReturn SwitchBody::compile(ts::Interpreter* interpreter) {
	return {};
}
