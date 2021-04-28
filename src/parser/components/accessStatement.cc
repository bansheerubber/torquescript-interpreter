#include "accessStatement.h"
#include "mathExpression.h"

bool AccessStatement::DatablockAsSymbol = false;

bool AccessStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser, bool useKeyword) {
	Token token = tokenizer->peekToken();
	return (
		token.type == LOCAL_VARIABLE
		|| token.type == GLOBAL_VARIABLE
		|| token.type == SYMBOL
		|| (useKeyword && tokenizer->isAlphabeticalKeyword(token.type))
	);
}

AccessStatement* AccessStatement::Parse(
	Component* firstValue,
	Component* parent,
	Tokenizer* tokenizer,
	Parser* parser,
	bool useKeyword
) {
	AccessStatement* output = new AccessStatement(parser);
	output->parent = parent;

	Token token = tokenizer->peekToken();
	if(firstValue != nullptr) {
		output->elements.push_back({
			component: firstValue,
		});
		firstValue->setParent(output);
	}
	else if(
		token.type == LOCAL_VARIABLE
		|| token.type == GLOBAL_VARIABLE
		|| token.type == SYMBOL
		|| (useKeyword && tokenizer->isAlphabeticalKeyword(token.type))
	) { // we read in a single variable at the start of the chain
		output->elements.push_back({
			token: tokenizer->getToken(),
		});
	}
	
	bool expectingArrayOrCall = true;
	while(!tokenizer->eof()) {
		token = tokenizer->peekToken();
		if(token.type == LEFT_BRACE) {
			if(expectingArrayOrCall) {
				output->elements.push_back({
					isArray: true,
					component: ArrayStatement::Parse(output, tokenizer, parser),
				});
				expectingArrayOrCall = false;
			}
			else {
				parser->error("was not expecting array access");
			}
		}
		else if(token.type == LEFT_PARENTHESIS) {
			if(expectingArrayOrCall) {
				output->elements.push_back({
					isCall: true,
					component: CallStatement::Parse(output, tokenizer, parser),
				});
				expectingArrayOrCall = false;
			}
			else {
				parser->error("was not expecting call");
			}
		}
		else if(token.type == MEMBER_CHAIN) {
			tokenizer->getToken(); // absorb the token we peeked
			output->elements.push_back({
				token: token,
			});
			expectingArrayOrCall = true;
		}
		else {
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
		else if(element.component != nullptr) {
			output += element.component->print();
		}
		else if(element.token.type) {
			output += element.token.lexeme;
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
		if(element.token.type == MEMBER_CHAIN) {
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