#include "ifBody.h"
#include "../interpreter/interpreter.h"

#include "elseIfBody.h"
#include "elseBody.h"

bool IfBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == IF;
}

IfBody* IfBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	IfBody* output = new IfBody(parser);
	output->parent = body;
	
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

	// see if we have else/else if stuff
	Body* next = output;
	while(!tokenizer->eof()) {
		if(ElseIfBody::ShouldParse(tokenizer, parser)) {
			Body* component = ElseIfBody::Parse(body, tokenizer, parser);

			// set next
			if(next->getType() == ELSE_IF_STATEMENT) {
				((ElseIfBody*)next)->next = component;
			}
			else if(next->getType() == IF_STATEMENT) {
				((IfBody*)next)->next = component;
			}

			next = component;
		}
		else if(ElseBody::ShouldParse(tokenizer, parser)) {
			Body* component = ElseBody::Parse(body, tokenizer, parser);

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
	string output = "if(" + this->conditional->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;

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

ts::InstructionReturn IfBody::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// final NOOP statement in if statement
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	// conditional statement for if statement
	ts::Instruction* conditionalJump = new ts::Instruction();
	conditionalJump->type = ts::instruction::JUMP_IF_FALSE;
	conditionalJump->jumpIfFalse.instruction = noop;
	conditionalJump->jumpIfFalse.pop = true;

	output.add(this->conditional->compile(interpreter, context));
	output.add(conditionalJump);
	
	for(Component* component: this->children) {
		output.add(component->compile(interpreter, context));
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
				ElseIfBodyCompiled compiled = ((ElseIfBody*)next)->compileElseIf(interpreter, context);
				compiled.lastJump->jump.instruction = noop;
				output.add(compiled.output);

				lastConditionalJump->jumpIfFalse.instruction = compiled.output.first; // weave together jumps with first of chain
				lastConditionalJump = compiled.conditionalJump;
				next = ((ElseIfBody*)next)->next;
			}
			else if(next->getType() == ELSE_STATEMENT) {
				ts::InstructionReturn compiled = next->compile(interpreter, context);
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
