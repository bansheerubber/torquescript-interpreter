#include "ifBody.h"
#include "../interpreter/interpreter.h"

#include "elseIfBody.h"
#include "elseBody.h"

bool IfBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == IF;
}

IfBody* IfBody::Parse(Body* body, ts::Engine* engine) {
	IfBody* output = new IfBody(engine);
	output->parent = body;
	
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

	// see if we have else/else if stuff
	Body* next = output;
	while(!engine->tokenizer->eof()) {
		if(ElseIfBody::ShouldParse(engine)) {
			Body* component = ElseIfBody::Parse(body, engine);

			// set next
			if(next->getType() == ELSE_IF_STATEMENT) {
				((ElseIfBody*)next)->next = component;
			}
			else if(next->getType() == IF_STATEMENT) {
				((IfBody*)next)->next = component;
			}

			next = component;
		}
		else if(ElseBody::ShouldParse(engine)) {
			Body* component = ElseBody::Parse(body, engine);

			// set next
			if(next->getType() == ELSE_IF_STATEMENT) {
				((ElseIfBody*)next)->next = component;
			}
			else if(next->getType() == IF_STATEMENT) {
				((IfBody*)next)->next = component;
			}

			break;
		}
		else {
			break;
		}
	}
	
	return output;
}

string IfBody::print() {
	string output = "if(" + this->conditional->print() + ")" + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "}" + this->engine->parser->newLine;

	Body* next = this->next;
	while(next != nullptr) {
		output += next->print();
		if(next->getType() == ELSE_IF_STATEMENT) {
			next = ((ElseIfBody*)next)->next;
		}
		else {
			next = nullptr;
		}
	}

	return output;
}

string IfBody::printJSON() {
	string output = "{\"type\":\"IF_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + ",\"followers\":[";
	Body* next = this->next;
	while(next != nullptr) {
		output += next->printJSON();
		if(next->getType() == ELSE_IF_STATEMENT) {
			output += ",";
			next = ((ElseIfBody*)next)->next;
		}
		else {
			next = nullptr;
		}
	}

	if(output.back() == ',') {
		output.pop_back();
	}

	output += "]}";
	return output;
}

ts::InstructionReturn IfBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in if statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// conditional statement for if statement
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = noop;
	conditionalJump->jumpIfFalse.pop = true;

	output.add(this->conditional->compile(engine, context));
	output.add(conditionalJump);
	
	for(Component* component: this->children) {
		output.add(component->compile(engine, context));
	}

	if(this->next != nullptr) {
		// jump at the end of the if statement, prevents else/else if
		ts::Instruction* jump = new ts::Instruction();
		jump->type = ts::instruction::JUMP;
		jump->jump.instruction = noop;
		output.add(jump);

		// iterate through else/else if chain
		ts::Instruction* lastConditionalJump = conditionalJump;
		Body* next = this->next;
		while(next != nullptr) {
			if(next->getType() == ELSE_IF_STATEMENT) {
				ElseIfBodyCompiled compiled = ((ElseIfBody*)next)->compileElseIf(engine, context);
				compiled.lastJump->jump.instruction = noop;
				output.add(compiled.output);

				lastConditionalJump->jumpIfFalse.instruction = compiled.output.first; // weave together jumps with first of chain
				lastConditionalJump = compiled.conditionalJump;
				next = ((ElseIfBody*)next)->next;
			}
			else if(next->getType() == ELSE_STATEMENT) {
				ts::InstructionReturn compiled = next->compile(engine, context);
				lastConditionalJump->jumpIfFalse.instruction = compiled.first;
				lastConditionalJump = nullptr;
				output.add(compiled);
				next = nullptr;
			}
		}

		if(lastConditionalJump) {
			lastConditionalJump->jumpIfFalse.instruction = noop;
		}
	}

	output.add(noop);

	return output;
}
