#include "forBody.h"
#include "../interpreter/interpreter.h"

bool ForBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == FOR;
}

ForBody* ForBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	ForBody* output = new ForBody(parser);
	output->parent = body;
	
	parser->expectToken(FOR);
	parser->expectToken(LEFT_PARENTHESIS);

	// parse initialization
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' initialization");
	}
	output->initialization = Component::Parse(output, tokenizer, parser);

	parser->expectToken(SEMICOLON);

	// parse conditional
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' conditional");
	}
	output->conditional = Component::Parse(output, tokenizer, parser);

	parser->expectToken(SEMICOLON);

	// parse increment
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression in 'for' increment");
	}
	output->increment = Component::Parse(output, tokenizer, parser);

	parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line for statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the for statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ForBody::print() {
	string output = "for(" + this->initialization->print() + ";" + this->parser->space + this->conditional->print() + ";" + this->parser->space + this->increment->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

string ForBody::printJSON() {
	return "{\"type\":\"FOR_STATEMENT\",\"initialization\":" + this->initialization->printJSON() + ",\"conditional\":" + this->conditional->printJSON() + ",\"increment\":" + this->increment->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ForBody::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in for statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// add variable initialization
	output.add(this->initialization->compile(interpreter, context));

	// add conditional
	ts::InstructionReturn compiledConditional = this->conditional->compile(interpreter, context);
	output.add(compiledConditional);

	// add increment
	ts::InstructionReturn compiledIncrement = this->increment->compile(interpreter, context);

	// add conditional jump
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = noop;
	conditionalJump->jumpIfFalse.pop = true;
	output.add(conditionalJump);

	// add the body
	for(Component* component: this->children) {
		output.add(component->compile(interpreter, (ts::CompilationContext){
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
