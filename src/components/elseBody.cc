#include "elseBody.h"
#include "../interpreter/interpreter.h"

bool ElseBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == ELSE;
}

ElseBody* ElseBody::Parse(Body* body, ts::Engine* engine) {
	ElseBody* output = new ElseBody(engine);
	output->parent = body;

	engine->parser->expectToken(ELSE);

	// handle one line else statements
	if(engine->tokenizer->peekToken().type != LEFT_BRACKET) {
		Component::ParseBody(output, engine, true);
	}
	else {
		engine->tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, engine); // parse the body of the else statement
		engine->parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ElseBody::print() {
	string output = "else" + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "}" + this->engine->parser->newLine;
	return output;
}

string ElseBody::printJSON() {
	return "{\"type\":\"ELSE_STATEMENT\",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ElseBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	if(this->children.size() == 0) {
		ts::Instruction* noop = new ts::Instruction();
		noop->type = ts::instruction::NOOP;	
		output.add(noop);
	}
	else {
		for(Component* component: this->children) {
			output.add(component->compile(engine, context));
		}
	}
	return output;
}
