#include "forBody.h"
#include "../interpreter/interpreter.h"

bool ForBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == FOR;
}

ForBody* ForBody::Parse(Body* body, ts::Engine* engine) {
	ForBody* output = new ForBody(engine);
	output->parent = body;
	
	engine->parser->expectToken(FOR);
	engine->parser->expectToken(LEFT_PARENTHESIS);

	// parse initialization
	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression in 'for' initialization");
	}
	output->initialization = Component::Parse(output, engine);

	engine->parser->expectToken(SEMICOLON);

	// parse conditional
	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression in 'for' conditional");
	}
	output->conditional = Component::Parse(output, engine);

	engine->parser->expectToken(SEMICOLON);

	// parse increment
	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression in 'for' increment");
	}
	output->increment = Component::Parse(output, engine);

	engine->parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line for statements
	if(engine->tokenizer->peekToken().type != LEFT_BRACKET) {
		Component::ParseBody(output, engine, true);
	}
	else {
		engine->tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, engine); // parse the body of the for statement
		engine->parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ForBody::print() {
	string output = "for(" + this->initialization->print() + ";" + this->engine->parser->space + this->conditional->print() + ";" + this->engine->parser->space + this->increment->print() + ")" + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "}" + this->engine->parser->newLine;
	return output;
}

string ForBody::printJSON() {
	return "{\"type\":\"FOR_STATEMENT\",\"initialization\":" + this->initialization->printJSON() + ",\"conditional\":" + this->conditional->printJSON() + ",\"increment\":" + this->increment->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ForBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in for statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// add variable initialization
	output.add(this->initialization->compile(engine, context));

	// add conditional
	ts::InstructionReturn compiledConditional = this->conditional->compile(engine, context);
	output.add(compiledConditional);

	// add increment
	ts::InstructionReturn compiledIncrement = this->increment->compile(engine, context);

	// add conditional jump
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = noop;
	conditionalJump->jumpIfFalse.pop = true;
	output.add(conditionalJump);

	// add the body
	for(Component* component: this->children) {
		output.add(component->compile(engine, (ts::CompilationContext){
			loop: this,
			package: nullptr,
			scope: context.scope,
		}));
	}

	for(ts::InstructionReturn &compiled: this->breaks) {
		compiled.first->jump.instruction = noop;
	}

	for(ts::InstructionReturn &compiled: this->continues) {
		compiled.first->jump.instruction = compiledIncrement.first;
	}

	// add the increment
	output.add(compiledIncrement);

	// add jump to conditional
	ts::Instruction* jump = new ts::Instruction();
	jump->type = ts::instruction::JUMP;
	jump->jump.instruction = compiledConditional.first;
	output.add(jump);

	// add final noop
	output.add(noop);

	return output;
}
