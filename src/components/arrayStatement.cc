#include "arrayStatement.h"

bool ArrayStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == LEFT_BRACE;
}

ArrayStatement* ArrayStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	ArrayStatement* output = new ArrayStatement(parser);
	output->parent = parent;
	
	parser->expectToken(LEFT_BRACE);

	bool expectingComma = false;
	while(!tokenizer->eof()) {
		if(!expectingComma) {
			if(Component::ShouldParse(output, tokenizer, parser)) {
				output->elements.push_back({
					component: Component::Parse(output, tokenizer, parser),
				});
				expectingComma = true;
			}
			else {
				parser->error("expected expression in array access");
			}
		}
		else {
			Token token = parser->expectToken(COMMA, RIGHT_BRACE);
			if(token.type == COMMA) {
				output->elements.push_back({
					isComma: true,
				});
				expectingComma = false;
			}
			else { // we got the right brace, quit out
				break;
			}
		}
	}
	
	return output;
}

int ArrayStatement::getDimensions() {
	int count = 0;
	for(ArrayElement &element: this->elements) {	
		if(element.component != nullptr) {
			count++;
		}
	}
	return count;
}

string ArrayStatement::print() {
	string output = "[";
	for(ArrayElement element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->print();
		}
		else if(element.isComma) {
			output += "," + this->parser->space;
		}
	}
	output += "]";
	return output;
}

ts::InstructionReturn ArrayStatement::compile() {
	ts::InstructionReturn output;

	for(ArrayElement &element: this->elements) {
		if(!element.isComma) {
			ts::InstructionReturn compiled = element.component->compile();
			
			if(output.first == nullptr) {
				output.first = compiled.first;
				output.last = compiled.last;
			}
			else {
				output.last->next = compiled.first;
				output.last = compiled.last;
			}
		}
	}

	return output;
}
