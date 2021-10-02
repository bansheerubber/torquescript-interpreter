#include "defaultBody.h"
#include "../interpreter/interpreter.h"

bool DefaultBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == DEFAULT;
}

DefaultBody* DefaultBody::Parse(Body* body, ts::Engine* engine) {
	DefaultBody* output = new DefaultBody(engine);
	output->parent = body;
	
	engine->parser->expectToken(DEFAULT);
	engine->parser->expectToken(COLON);

	Component::ParseBody(output, engine); // parse the body of the case statement
	
	return output;
}

string DefaultBody::print() {
	string output = "default:" + this->engine->parser->newLine;
	output += this->printBody();
	return output;
}

string DefaultBody::printJSON() {
	return "{\"type\":\"DEFAULT_STATEMENT\",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn DefaultBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	for(Component* component: this->children) {
		output.add(component->compile(engine, context));
	}

	if(output.first == nullptr) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::NOOP;
		output.add(instruction);
	}

	return output;
}
