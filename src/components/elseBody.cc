#include "elseBody.h"
#include "../interpreter/interpreter.h"

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
	string output = "else" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

string ElseBody::printJSON() {
	return "{\"type\":\"ELSE_STATEMENT\",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ElseBody::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	ts::InstructionReturn output;
	if(this->children.size() == 0) {
		ts::Instruction* noop = new ts::Instruction();
		noop->type = ts::instruction::NOOP;	
		output.add(noop);
	}
	else {
		for(Component* component: this->children) {
			output.add(component->compile(interpreter, scope));
		}
	}
	return output;
}
