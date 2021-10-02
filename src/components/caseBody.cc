#include "caseBody.h"
#include "../interpreter/interpreter.h"

bool CaseBody::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == CASE;
}

CaseBody* CaseBody::Parse(Body* body, ts::Engine* engine) {
	CaseBody* output = new CaseBody(engine);
	output->parent = body;
	
	engine->parser->expectToken(CASE);

	bool expectingOr = false;
	while(!engine->tokenizer->eof()) {
		Token token = engine->tokenizer->peekToken();
		if(!expectingOr) {
			if(!Component::ShouldParse(output, engine)) {
				engine->parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'case' conditional");
			}
			
			output->conditionals.push_back((CaseElement){
				component: Component::Parse(output, engine),
			});
			expectingOr = true;
		}
		else if(token.type == OR) {
			engine->tokenizer->getToken(); // absorb or
			output->conditionals.push_back((CaseElement){
				isOr: true,
			});
			expectingOr = false;
		}
		else if(token.type == COLON) {
			engine->parser->expectToken(COLON);
			break;
		}
		else {
			engine->parser->error("unexpected token '%s' while parsing case statement conditional", token.lexeme.c_str());
		}
	}

	Component::ParseBody(output, engine); // parse the body of the case statement
	
	return output;
}

string CaseBody::print() {
	string output = "case ";
	for(CaseElement element: this->conditionals) {
		if(element.component != nullptr) {
			output += element.component->print();
		}
		else {
			output += " or ";
		}
	}
	output += ":" + this->engine->parser->newLine;
	output += this->printBody();
	return output;
}

string CaseBody::printJSON() {
	string output = "{\"type\":\"CASE_STATEMENT\",\"conditionals\":";
	string comma = this->conditionals.size() != 1 ? "," : "";
	for(CaseElement element: this->conditionals) {
		if(element.component != nullptr) {
			output += element.component->printJSON() + comma;
		}
	}

	if(output.back() == ',') {
		output.pop_back();
	}

	output += ",\"body\":" + this->printJSONBody() + "}";
	return output;
}

ts::InstructionReturn CaseBody::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	for(Component* component: this->children) {
		output.add(component->compile(engine, context));
	}

	if(output.first == nullptr) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::NOOP;
		output.add(instruction);
	}

	return output;
}
