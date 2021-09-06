#include "inlineConditional.h"
#include "../interpreter/interpreter.h"

bool InlineConditional::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == QUESTION_MARK;
}

InlineConditional* InlineConditional::Parse(Component* leftHandSide, Component* parent, ts::Engine* engine) {
	InlineConditional* output = new InlineConditional(engine);
	output->parent = parent;
	output->leftHandSide = leftHandSide;
	leftHandSide->setParent(output->leftHandSide);

	engine->parser->expectToken(QUESTION_MARK);

	// parse a component
	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for inline conditional true condition");
	}
	output->ifTrue = Component::Parse(output, engine);

	engine->parser->expectToken(COLON);

	// parse a component
	if(!Component::ShouldParse(output, engine)) {
		engine->parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for inline conditional false condition");
	}
	output->ifFalse = Component::Parse(output, engine);

	return output;
}

string InlineConditional::print() {
	return this->leftHandSide->print() + this->engine->parser->space + "?" + this->engine->parser->space + this->ifTrue->print() + " : " + this->ifFalse->print();
}

string InlineConditional::printJSON() {
	return "{\"type\":\"INLINE_CONDITIONAL\",\"conditional\":" + this->leftHandSide->printJSON() + ",\"ifTrue\":" + this->ifTrue->printJSON() + ",\"ifFalse\":" + this->ifFalse->printJSON() + "}";
}

ts::InstructionReturn InlineConditional::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in inline statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// compile if true statement
	ts::InstructionReturn ifTrue = this->ifTrue->compile(engine, context);

	// compile if false statement
	ts::InstructionReturn ifFalse = this->ifFalse->compile(engine, context);

	// conditional statement for inline statement
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = ifFalse.first;
	conditionalJump->jumpIfFalse.pop = true;

	output.add(this->leftHandSide->compile(engine, context));
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
