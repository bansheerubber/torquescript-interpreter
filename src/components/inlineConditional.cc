#include "inlineConditional.h"
#include "../interpreter/interpreter.h"

bool InlineConditional::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == QUESTION_MARK;
}

InlineConditional* InlineConditional::Parse(Component* leftHandSide, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	InlineConditional* output = new InlineConditional(parser);
	output->parent = parent;
	output->leftHandSide = leftHandSide;
	leftHandSide->setParent(output->leftHandSide);

	parser->expectToken(QUESTION_MARK);

	// parse a component
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for inline conditional true condition");
	}
	output->ifTrue = Component::Parse(output, tokenizer, parser);

	parser->expectToken(COLON);

	// parse a component
	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for inline conditional false condition");
	}
	output->ifFalse = Component::Parse(output, tokenizer, parser);

	return output;
}

string InlineConditional::print() {
	return this->leftHandSide->print() + this->parser->space + "?" + this->parser->space + this->ifTrue->print() + " : " + this->ifFalse->print();
}

string InlineConditional::printJSON() {
	return "{\"type\":\"INLINE_CONDITIONAL\",\"conditional\":" + this->leftHandSide->printJSON() + ",\"ifTrue\":" + this->ifTrue->printJSON() + ",\"ifFalse\":" + this->ifFalse->printJSON() + "}";
}

ts::InstructionReturn InlineConditional::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in inline statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// compile if true statement
	ts::InstructionReturn ifTrue = this->ifTrue->compile(interpreter, context);

	// compile if false statement
	ts::InstructionReturn ifFalse = this->ifFalse->compile(interpreter, context);

	// conditional statement for inline statement
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = ifFalse.first;
	conditionalJump->jumpIfFalse.pop = true;

	output.add(this->leftHandSide->compile(interpreter, context));
	output.add(conditionalJump);

	output.add(ifTrue);

	// add jump to end once we're done with true
	ts::Instruction* jumpToEnd = new ts::Instruction();
	jumpToEnd->type = ts::instruction::JUMP;
	jumpToEnd->jump.instruction = noop;
	output.add(jumpToEnd);

	// add if false statement
	output.add(ifFalse);

	output.add(noop);

	return output;
}