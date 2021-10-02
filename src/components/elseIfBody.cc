#include "elseIfBody.h"
#include "../interpreter/interpreter.h"

bool ElseIfBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == ELSE && engine->tokenizer->peekToken(1).type == IF;
}

ElseIfBody* ElseIfBody::Parse(Body* body, ts::Engine* engine) {
	ElseIfBody* output = new ElseIfBody(engine);
	output->parent = body;

	engine->parser->expectToken(ELSE);
	engine->parser->expectToken(IF);
	engine->parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'if' conditional");
	}
	output->conditional = Component::Parse(output, engine);
	
	engine->parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line if statements
	if(engine->tokenizer->peekToken().type != LEFT_BRACKET) {
		Component::ParseBody(output, engine, true);
	}
	else {
		engine->tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, engine); // parse the body of the if statement
		engine->parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ElseIfBody::print() {
	string output = "else if(" + this->conditional->print() + ")" + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "}" + this->engine->parser->newLine;
	return output;
}

string ElseIfBody::printJSON() {
	return "{\"type\":\"ELSE_IF_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ElseIfBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	return this->compileElseIf(engine, context).output;
}

ElseIfBodyCompiled ElseIfBody::compileElseIf(ts::Engine* engine, ts::CompilationContext context) {
	ElseIfBodyCompiled compiled;

	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE; // the instruction this jumps to will be set by the if statement compilation
	conditionalJump->jumpIfFalse.pop = true;
	compiled.conditionalJump = conditionalJump;

	compiled.output.add(this->conditional->compile(engine, context));
	compiled.output.add(conditionalJump);

	for(Component* component: this->children) {
		compiled.output.add(component->compile(engine, context));
	}

	ts::Instruction* jump = new ts::Instruction();
	jump->type = ts::instruction::JUMP; // the instruction this jumps to will be set by the if statement compilation
	compiled.lastJump = jump;
	compiled.output.add(jump);

	return compiled;
}
