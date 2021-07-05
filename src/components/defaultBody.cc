#include "defaultBody.h"
#include "../interpreter/interpreter.h"

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

string DefaultBody::printJSON() {
	return "{\"type\":\"DEFAULT_STATEMENT\",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn DefaultBody::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;
	for(Component* component: this->children) {
		output.add(component->compile(interpreter, context));
	}

	if(output.first == nullptr) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::NOOP;
		output.add(instruction);
	}

	return output;
}
