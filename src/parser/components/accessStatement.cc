#include "accessStatement.h"

bool AccessStatement::DatablockAsSymbol = false;

bool AccessStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	Token token = tokenizer->peekToken();
	return (
		token.type == LOCAL_VARIABLE
		|| token.type == GLOBAL_VARIABLE
		|| token.type == SYMBOL
		|| (AccessStatement::DatablockAsSymbol && token.type == DATABLOCK)
	);
}

AccessStatement* AccessStatement::Parse(Component* firstValue, Component* parent, Tokenizer* tokenizer, Parser* parser, bool oneSymbol) {
	AccessStatement* output = new AccessStatement();
	output->parent = parent;

	bool expectingDot = false;
	bool expectingArrayOrCall = false;
	bool expectingVariable = true;
	if(firstValue != nullptr) {
		output->elements.push_back({
			component: firstValue,
		});
		firstValue->setParent(output);
		
		expectingDot = true;
		expectingArrayOrCall = true;
		expectingVariable = false;
	}
	
	Token token;
	while((token = tokenizer->getToken()).type) {
		if(
			token.type == LOCAL_VARIABLE
			|| token.type == GLOBAL_VARIABLE
			|| token.type == SYMBOL
			|| (AccessStatement::DatablockAsSymbol && token.type == DATABLOCK)
		) {
			if(expectingVariable) {
				output->elements.push_back({
					token: token,
				});
				expectingDot = true;
				expectingArrayOrCall = true;
				expectingVariable = false;

				if(oneSymbol) {
					break; // break after reading one symbol
				}
			}
			else {
				parser->error("two variables next to each other");
			}
		}
		else if(token.type == LEFT_BRACE) {
			tokenizer->unGetToken();
			if(expectingArrayOrCall) {
				output->elements.push_back({
					isArray: true,
					component: ArrayStatement::Parse(output, tokenizer, parser),
				});
				expectingDot = true;
				expectingArrayOrCall = false;
				expectingVariable = false;
			}
			else {
				parser->error("was not expecting array access");
			}
		}
		else if(token.type == LEFT_PARENTHESIS) {
			tokenizer->unGetToken();
			if(expectingArrayOrCall) {
				output->elements.push_back({
					isCall: true,
					component: CallStatement::Parse(output, tokenizer, parser),
				});
				expectingDot = true;
				expectingArrayOrCall = false;
				expectingVariable = false;
			}
			else {
				parser->error("was not expecting call");
			}
		}
		else if(token.type == DOT) {
			if(expectingDot) {
				output->elements.push_back({
					isDot: true,
				});
				expectingDot = false;
				expectingArrayOrCall = false;
				expectingVariable = true;
			}
			else {
				parser->error("was not expecting member chain");
			}
		}
		else {
			if(expectingVariable) {
				parser->error("incomplete member chain");
			}
			
			// TODO could make this stricter??
			tokenizer->unGetToken();
			break;
		}
	}
	return output;
}

string AccessStatement::print() {
	string output;
	for(AccessElement element: this->elements) {
		if(element.token.type == LOCAL_VARIABLE) {
			output += "%" + element.token.lexeme;
		}
		else if(element.token.type == GLOBAL_VARIABLE) {
			output += "$" + element.token.lexeme;
		}
		else if(element.token.type == SYMBOL) {
			output += element.token.lexeme;
		}
		else if(element.isDot) {
			output += ".";
		}
		else if(element.component != nullptr) {
			output += element.component->print();
		}
	}
	
	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}

	return output;
}

bool AccessStatement::isLocalVariable() {
	return this->elements.front().token.type == LOCAL_VARIABLE;
}

bool AccessStatement::isGlobalVariable() {
	return this->elements.front().token.type == GLOBAL_VARIABLE;
}

bool AccessStatement::hasChain() {
	for(AccessElement element: this->elements) {
		if(element.isDot) {
			return true;
		}
	}
	return false;
}

bool AccessStatement::hasArray() {
	for(AccessElement element: this->elements) {
		if(element.isArray) {
			return true;
		}
	}
	return false;
}

bool AccessStatement::hasCall() {
	for(AccessElement element: this->elements) {
		if(element.isCall) {
			return true;
		}
	}
	return false;
}

int AccessStatement::chainSize() {
	return this->elements.size();
}

// we cannot assign to a function call or to a symbol
bool AccessStatement::IsValidLvalue() {
	if(this->elements.size() == 1 && this->elements.back().token.type == SYMBOL) {
		return false;
	}
	else if(this->elements.back().isCall) {
		return false;
	}
	return true;
}