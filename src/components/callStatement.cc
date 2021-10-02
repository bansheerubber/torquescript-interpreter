#include "callStatement.h"
#include "../interpreter/interpreter.h"

bool CallStatement::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == LEFT_PARENTHESIS;
}

CallStatement* CallStatement::Parse(Component* parent, ts::Engine* engine) {
	CallStatement* output = new CallStatement(engine);
	output->parent = parent;
	
	engine->parser->expectToken(LEFT_PARENTHESIS);

	bool expectingComma = false;
	while(!engine->tokenizer->eof()) {
		if(!expectingComma) {
			if(Component::ShouldParse(output, engine)) {
				output->elements.push_back((CallElement){
					component: Component::Parse(output, engine),
				});
				expectingComma = true;
			}
			else if(engine->tokenizer->peekToken().type == RIGHT_PARENTHESIS) { // there's no arguments
				engine->tokenizer->getToken(); // absorb right parenthesis
				break;
			}
			else {
				engine->parser->error("could not parse component in argument list");
			}
		}
		else {
			Token token = engine->parser->expectToken(COMMA, RIGHT_PARENTHESIS);
			if(token.type == COMMA) {
				output->elements.push_back((CallElement){
					isComma: true,
				});
				expectingComma = false;
			}
			else { // we got the right parenthesis, quit out
				break;
			}
		}
	}
	
	return output;
}

string CallStatement::print() {
	string output = "(";
	for(CallElement element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->print();
		}
		else if(element.isComma) {
			output += "," + this->engine->parser->space;
		}
	}
	output += ")";
	return output;
}

string CallStatement::printJSON() {
	string output = "{\"type\":\"CALL_STATEMENT\",\"arguments\":[";
	for(CallElement element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->printJSON();
		}
		else if(element.isComma) {
			output += ",";
		}
	}

	output += "]}";
	return output;
}

pair<
	vector<CallElement>::iterator,
	vector<CallElement>::iterator
> CallStatement::getElements() {
	return make_pair(this->elements.begin(), this->elements.end());
}

CallElement &CallStatement::getElement(size_t index) {
	return this->elements[index];
}

size_t CallStatement::getElementCount() {
	size_t count = 0;
	for(CallElement element: this->elements) {
		if(element.component != nullptr) {
			count++;
		}
	}
	return count;
}

// upon compilation, push values of the variables
ts::InstructionReturn CallStatement::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	for(CallElement &element: this->elements) {
		if(!element.isComma) {
			output.add(element.component->compile(engine, context));
		}
	}
	return output;
}
