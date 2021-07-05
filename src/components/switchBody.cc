#include "switchBody.h"
#include "../interpreter/interpreter.h"

#include "caseBody.h"
#include "defaultBody.h"

bool SwitchBody::ShouldParse(Tokenizer* tokenizer, class Parser* parser) {
	Token token = tokenizer->peekToken();
	return token.type == SWITCH || token.type == STRING_SWITCH;
}

SwitchBody* SwitchBody::Parse(Body* body, Tokenizer* tokenizer, class Parser* parser) {
	SwitchBody* output = new SwitchBody(parser);
	output->parent = body;

	output->switchType = parser->expectToken(SWITCH, STRING_SWITCH);
	parser->expectToken(LEFT_PARENTHESIS);

	if(!Component::ShouldParse(output, tokenizer, parser)) {
		parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'switch' conditional");
	}
	output->conditional = Component::Parse(output, tokenizer, parser);
	
	parser->expectToken(RIGHT_PARENTHESIS);
	parser->expectToken(LEFT_BRACKET);

	while(!tokenizer->eof()) {
		Token token = tokenizer->peekToken();
		if(CaseBody::ShouldParse(tokenizer, parser)) {
			output->children.push_back(CaseBody::Parse(output, tokenizer, parser));
		}
		else if(DefaultBody::ShouldParse(tokenizer, parser)) {
			output->children.push_back(DefaultBody::Parse(output, tokenizer, parser));
		}
		else if(token.type == RIGHT_BRACKET) {
			break;
		}
		else {
			parser->error("unexpected token '%s' in switch statement body", token.lexeme.c_str());
		}
	}

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string SwitchBody::print() {
	string output = "switch(" + this->conditional->print() + ")" + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

string SwitchBody::printJSON() {
	return "{\"type\":\"SWITCH_STATEMENT\",\"conditional\":" + this->conditional->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn SwitchBody::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;
	vector<CaseBody*> caseBodies;
	DefaultBody* defaultBody = nullptr;

	for(Component* component: this->children) {
		if(component->getType() == CASE_STATEMENT) {
			caseBodies.push_back((CaseBody*)component);
		}
		else if(component->getType() == DEFAULT_STATEMENT) {
			defaultBody = (DefaultBody*)component;
		}
	}
	
	ts::Instruction* noop = new ts::Instruction();
	noop->type = ts::instruction::NOOP;

	ts::InstructionReturn lastConditionals;
	for(CaseBody* body: caseBodies) {
		// for torquescript compatibility, we have to evaluate the switch main conditional every case statement
		ts::InstructionReturn conditionals;
		ts::InstructionReturn compiledBody = body->compile(interpreter, context);
		for(CaseElement element: body->conditionals) {
			if(element.isOr) {
				continue;
			}

			// should be a value that was pushed onto the stack
			ts::InstructionReturn caseValue = element.component->compile(interpreter, context);

			// should be a value that was also pushed onto the stack
			ts::InstructionReturn switchConditional = this->conditional->compile(interpreter, context);

			ts::Instruction* comparison = new ts::Instruction();

			if(this->switchType.type == STRING_SWITCH) {
				comparison->type = ts::instruction::MATH_STRING_EQUAL;
			}
			else {
				comparison->type = ts::instruction::MATH_EQUAL;
			}

			comparison->mathematics.lvalueEntry = ts::Entry();
			comparison->mathematics.lvalueEntry.type = ts::entry::INVALID;
			comparison->mathematics.rvalueEntry = ts::Entry();
			comparison->mathematics.rvalueEntry.type = ts::entry::INVALID;
			comparison->mathematics.lvalueStackIndex = -1;
			comparison->mathematics.rvalueStackIndex = -1;

			if(conditionals.last != nullptr) {
				conditionals.last->type = ts::instruction::JUMP_IF_TRUE;
				conditionals.last->jumpIfTrue.pop = true;
				conditionals.last->jumpIfTrue.instruction = compiledBody.first; // if we're an or statement, jump to the next or
			}

			conditionals.add(caseValue);
			conditionals.add(switchConditional);
			conditionals.add(comparison);

			ts::Instruction* jumpIfFalse = new ts::Instruction();
			jumpIfFalse->type = ts::instruction::JUMP_IF_FALSE;
			jumpIfFalse->jumpIfFalse.pop = true;
			jumpIfFalse->jumpIfFalse.instruction = nullptr;

			conditionals.add(jumpIfFalse);
		}

		output.add(conditionals);
		output.add(compiledBody);

		ts::Instruction* jumpToEnd = new ts::Instruction();
		jumpToEnd->type = ts::instruction::JUMP;
		jumpToEnd->jump.instruction = noop;
		output.add(jumpToEnd);

		if(lastConditionals.last != nullptr) {
			lastConditionals.last->jumpIfFalse.instruction = conditionals.first;
		}

		lastConditionals = conditionals;
	}

	if(defaultBody != nullptr) {
		ts::InstructionReturn compiledDefault = defaultBody->compile(interpreter, context);
		output.add(compiledDefault);

		if(lastConditionals.last != nullptr) {
			lastConditionals.last->jumpIfFalse.instruction = compiledDefault.first;
		}
	}
	else if(lastConditionals.last != nullptr) {
		lastConditionals.last->jumpIfFalse.instruction = noop;
	}

	output.add(noop);
	
	return output;
}
