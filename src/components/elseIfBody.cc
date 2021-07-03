#include "elseIfBody.h"
#include "../interpreter/interpreter.h"

bool ElseIfBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == ELSE && tokenizer->peekToken(1).type == IF;
}

ElseIfBody* ElseIfBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	ElseIfBody* output = new ElseIfBody(parser);
	output->parent = body;

	parser->expectToken(ELSE);
	parser->expectToken(IF);
	parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'if' conditional");
	}
	output->conditional = Component::Parse(output, tokenizer, parser);
	
	parser->expectToken(RIGHT_PARENTHESIS);

	// handle one line if statements
	if(
		tokenizer->peekToken().type != LEFT_BRACKET
	) {
		Component::ParseBody(output, tokenizer, parser, true);
	}
	else {
		tokenizer->getToken(); // absorb bracket
		Component::ParseBody(output, tokenizer, parser); // parse the body of the if statement
		parser->expectToken(RIGHT_BRACKET);
	}
	
	return output;
}

string ElseIfBody::print() {
	string output = "else if(" + this->conditional->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

string ElseIfBody::printJSON() {
	return "{\"type\":\"ELSE_IF_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn ElseIfBody::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	return this->compileElseIf(interpreter, scope).output;
}

ElseIfBodyCompiled ElseIfBody::compileElseIf(ts::Interpreter* interpreter, ts::Scope* scope) {
	ElseIfBodyCompiled compiled;

	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE; // the instruction this jumps to will be set by the if statement compilation
	conditionalJump->jumpIfFalse.pop = true;
	compiled.conditionalJump = conditionalJump;

	compiled.output.add(this->conditional->compile(interpreter, scope));
	compiled.output.add(conditionalJump);

	for(Component* component: this->children) {
		compiled.output.add(component->compile(interpreter, scope));
	}

	ts::Instruction* jump = new ts::Instruction();
	jump->type = ts::instruction::JUMP; // the instruction this jumps to will be set by the if statement compilation
	compiled.lastJump = jump;
	compiled.output.add(jump);

	return compiled;
}