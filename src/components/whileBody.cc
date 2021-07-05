#include "whileBody.h"
#include "../interpreter/interpreter.h"

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

string WhileBody::printJSON() {
	return "{\"type\":\"WHILE_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn WhileBody::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;
	
	// final NOOP statement in while statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// add conditional
	ts::InstructionReturn compiledConditional = this->conditional->compile(interpreter, context);
	output.add(compiledConditional);

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
