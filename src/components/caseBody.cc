#include "caseBody.h"
#include "../interpreter/interpreter.h"

bool CaseBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == CASE;
}

CaseBody* CaseBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	CaseBody* output = new CaseBody(parser);
	output->parent = body;
	
	parser->expectToken(CASE);

	bool expectingOr = false;
	while(!tokenizer->eof()) {
		Token token = tokenizer->peekToken();
		if(!expectingOr) {
			if(!Component::ShouldParse(output, tokenizer, parser)) {
				parser->error("expected evaluateable expression, string literal, number literal, or boolean literal for 'case' conditional");
			}
			
			output->conditionals.push_back({
				component: Component::Parse(output, tokenizer, parser),
			});
			expectingOr = true;
		}
		else if(token.type == OR) {
			tokenizer->getToken(); // absorb or
			output->conditionals.push_back({
				isOr: true,
			});
			expectingOr = false;
		}
		else if(token.type == COLON) {
			parser->expectToken(COLON);
			break;
		}
		else {
			parser->error("unexpected token '%s' while parsing case statement conditional", token.lexeme.c_str());
		}
	}

	Component::ParseBody(output, tokenizer, parser); // parse the body of the case statement
	
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
	output += ":" + this->parser->newLine;
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

ts::InstructionReturn CaseBody::compile(ts::Interpreter* interpreter) {
	return {};
}
