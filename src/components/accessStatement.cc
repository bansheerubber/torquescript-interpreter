#include "accessStatement.h"
#include "mathExpression.h"
#include "../interpreter/interpreter.h"

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

string AccessStatement::printJSON() {
	string output = "{\"type\":\"ACCESS_STATEMENT\",";
	if(this->elements[0].token.type == LOCAL_VARIABLE) {
		output += "\"variableType\":\"LOCAL_VARIABLE\",";
	}
	else if(this->elements[0].token.type == GLOBAL_VARIABLE) {
		output += "\"variableType\":\"GLOBAL_VARIABLE\",";
	}
	else if(this->elements[0].token.type) {
		output += "\"variableType\":\"SYMBOL\",";
	}
	else if(this->elements[0].component != nullptr) {
		output += "\"variableType\":\"NONE\",";
	}

	output += "\"chain\":[";
	string comma = this->elements.size() != 1 ? "," : "";
	for(AccessElement &element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->printJSON() + comma;
		}
		else if(element.token.type) {
			output += "\"" + element.token.lexeme + "\"" + comma;
		}
	}

	if(output.back() == ',') {
		output.pop_back();
	}

	output += "]}";
	return output;
}

bool AccessStatement::isLocalVariable() {
	return this->elements.front().token.type == LOCAL_VARIABLE;
}

bool AccessStatement::isGlobalVariable() {
	return this->elements.front().token.type == GLOBAL_VARIABLE;
}

bool AccessStatement::isFunction() {
	if(
		this->elements.size() < 2
		|| this->elements.front().token.type != SYMBOL
		|| this->elements[1].component == nullptr
		|| this->elements[1].component->getType() != CALL_STATEMENT
	) {
		return false;
	}
	
	return true;
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

ts::InstructionReturn AccessStatement::compile(ts::Interpreter* interpreter) {
	return this->compileAccess(interpreter).output;
}

// create instructions that set up the stack for an array access/object property access instruction
AccessStatementCompiled AccessStatement::compileAccess(ts::Interpreter* interpreter) {
	AccessStatementCompiled c;

	if(this->isFunction()) { // compile a function call
		c.output.add(this->elements[1].component->compile(interpreter)); // push arguments

		// push the amount of arguments we just found
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::PUSH;
		instruction->push.entry.type = ts::entry::NUMBER;
		instruction->push.entry.setNumber(((CallStatement*)this->elements[1].component)->getElementCount());
		c.output.add(instruction);

		// build call instruction
		ts::Instruction* callFunction = new ts::Instruction();
		callFunction->type = ts::instruction::CALL_FUNCTION;
		new((void*)&callFunction->callFunction.name) string(this->elements[0].token.lexeme); // TODO move this initialization elsewhere
		callFunction->callFunction.cachedIndex = 0;
		callFunction->callFunction.isCached = false;
		callFunction->callFunction.isTSSL = false;
		c.output.add(callFunction);

		if(this->parent->requiresSemicolon(this)) { // if we do not assign/need the value of the function, just pop it
			ts::Instruction* pop = new ts::Instruction();
			pop->type = ts::instruction::POP;
			c.output.add(pop);
		}

		return c;
	}

	int count = 0;
	ts::Instruction* lastInstruction = nullptr;
	for(AccessElement &element: this->elements) {
		if(element.token.type == LOCAL_VARIABLE) {
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::LOCAL_ACCESS;
			instruction->localAccess.dimensions = 0;
			instruction->localAccess.hash = hash<string>{}(element.token.lexeme);
			new((void*)&instruction->localAccess.source) string(element.token.lexeme); // TODO move this initialization elsewhere

			c.lastAccess = instruction;

			lastInstruction = instruction;
		}
		else if(element.isArray) {
			lastInstruction->localAccess.dimensions = ((ArrayStatement*)element.component)->getDimensions();
			c.output.add(element.component->compile(interpreter));
		}
		else if(element.token.type == MEMBER_CHAIN) {
			if(lastInstruction != nullptr) {
				c.output.add(lastInstruction);
			}

			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::OBJECT_ACCESS;
			instruction->objectAccess.dimensions = 0;
			instruction->objectAccess.hash = hash<string>{}(element.token.lexeme);
			new((void*)&instruction->objectAccess.source) string(element.token.lexeme); // TODO move this initialization elsewhere

			c.lastAccess = instruction;

			lastInstruction = instruction;
		}
		count++;
	}

	if(lastInstruction != nullptr) {
		c.output.add(lastInstruction);
	}

	return c;
}

string AccessStatement::getVariableName() {
	return this->elements[0].token.lexeme;
}