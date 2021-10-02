#include "whileBody.h"
#include "../interpreter/interpreter.h"

bool WhileBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == WHILE;
}

WhileBody* WhileBody::Parse(Body* body, ts::Engine* engine) {
	WhileBody* output = new WhileBody(engine);
	output->parent = body;
	
	engine->parser->expectToken(WHILE);
	engine->parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'while' conditional");
	}
	output->conditional = Component::Parse(output, engine);
	
	engine->parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line while statements
	if(engine->tokenizer->peekToken().type != LEFT_BRACKET) {
		Component::ParseBody(output, engine, true);
	}
	else {
		engine->tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, engine); // parse the body of the while statement
		engine->parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string WhileBody::print() {
	string output = "while(" + this->conditional->print() + ")" + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "}" + this->engine->parser->newLine;
	return output;
}

string WhileBody::printJSON() {
	return "{\"type\":\"WHILE_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn WhileBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	
	// final NOOP statement in while statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// add conditional
	ts::InstructionReturn compiledConditional = this->conditional->compile(engine, context);
	output.add(compiledConditional);

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
		compiled.first->jump.instruction = compiledConditional.first;
	}

	// add jump to conditional
	ts::Instruction* jump = new ts::Instruction();
	jump->type = ts::instruction::JUMP;
	jump->jump.instruction = compiledConditional.first;
	output.add(jump);
	output.add(noop);
	
	return output;
}
