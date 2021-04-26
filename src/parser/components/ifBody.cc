#include "ifBody.h"
#include "elseIfBody.h"
#include "elseBody.h"

bool IfBody::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == IF;
}

IfBody* IfBody::Parse(Body* body, Tokenizer* tokenizer, Parser* parser) {
	IfBody* output = new IfBody();
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
	string output = "if(" + this->conditional->print() + ") {\n";
	output += this->printBody();
	output += "}\n";

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