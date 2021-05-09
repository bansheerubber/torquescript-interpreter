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

ts::InstructionReturn AccessStatement::compile() {
	return this->compileAccess().output;
}

// create instructions that set up the stack for an array access/object property access instruction
AccessStatementCompiled AccessStatement::compileAccess() {
	AccessStatementCompiled c;

	if(this->elements.size() == 1) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::LOCAL_ACCESS;
		new((void*)&instruction->localAccess.source) string(this->getVariableName()); // TODO move this initialization elsewhere

		c.output.first = instruction;
		c.output.last = instruction;
		c.lastAccess = instruction;
		return c;
	}

	int count = 0;
	ts::Instruction* lastInstruction = nullptr;
	for(AccessElement &element: this->elements) {
		if(element.token.type == LOCAL_VARIABLE) {
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::LOCAL_ACCESS;
			new((void*)&instruction->localAccess.source) string(element.token.lexeme); // TODO move this initialization elsewhere

			c.lastAccess = instruction;

			lastInstruction = instruction;
		}
		else if(element.isArray) {
			lastInstruction->localAccess.dimensions = ((ArrayStatement*)element.component)->getDimensions();

			ts::InstructionReturn compiled = element.component->compile();

			if(c.output.first == nullptr) {
				c.output.first = compiled.first;
				c.output.last = compiled.last;
			}
			else {
				c.output.last->next = compiled.first;
				c.output.last = compiled.last;
			}
		}
		else if(element.token.type == MEMBER_CHAIN) {
			if(lastInstruction != nullptr) {
				if(c.output.first == nullptr) {
					c.output.first = lastInstruction;
					c.output.last = lastInstruction;
				}
				else {
					c.output.last->next = lastInstruction;
					c.output.last = lastInstruction;
				}
			}
			
			continue;
		}
		count++;
	}

	if(lastInstruction != nullptr) {
		if(c.output.first == nullptr) {
			c.output.first = lastInstruction;
			c.output.last = lastInstruction;
		}
		else {
			c.output.last->next = lastInstruction;
			c.output.last = lastInstruction;
		}
	}

	return c;
}

string AccessStatement::getVariableName() {
	return this->elements[0].token.lexeme;
}