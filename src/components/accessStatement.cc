#include "accessStatement.h"
#include "../interpreter/interpreter.h"

#include "arrayStatement.h"
#include "callStatement.h"

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

	Token &token = tokenizer->peekToken();
	if(firstValue != nullptr) {
		output->elements.push_back((AccessElement){
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
		output->elements.push_back((AccessElement){
			token: tokenizer->getToken(),
		});
	}
	
	bool expectingArrayOrCall = true;
	while(!tokenizer->eof()) {
		token = tokenizer->peekToken();
		if(token.type == LEFT_BRACE) {
			if(expectingArrayOrCall) {
				output->elements.push_back((AccessElement){
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
				output->elements.push_back((AccessElement){
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
			output->elements.push_back((AccessElement){
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

bool AccessStatement::startsWithFunction() {
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

size_t AccessStatement::chainSize() {
	return this->elements.size();
}

// we cannot assign to a function call or to a symbol
bool AccessStatement::isValidLValue() {
	if(this->elements.size() == 1 && this->elements.back().token.type == SYMBOL) {
		return false;
	}
	else if(this->elements.back().isCall) {
		return false;
	}
	return true;
}

size_t AccessStatement::getStackIndex(ts::Scope* scope) {
	if(!this->isLocalVariable() || this->chainSize() != 1) {
		return -1;
	}
	else {
		string name = this->getVariableName();
		return scope->allocateVariable(name).stackIndex;
	}
}

ts::InstructionReturn AccessStatement::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	return this->compileAccess(interpreter, scope).output;
}

// create instructions that set up the stack for an array access/object property access instruction
AccessStatementCompiled AccessStatement::compileAccess(ts::Interpreter* interpreter, ts::Scope* scope) {
	AccessStatementCompiled c;

	auto iterator = this->elements.begin();

	if(this->startsWithFunction()) { // compile a function call
		c.output.add(this->elements[1].component->compile(interpreter, scope)); // push arguments

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
		new((void*)&callFunction->callFunction.nameSpace) string(""); // TODO move this initialization elsewhere
		callFunction->callFunction.cachedIndex = 0;
		callFunction->callFunction.cachedNamespaceIndex = 0;
		callFunction->callFunction.isCached = false;
		callFunction->callFunction.isNamespaceCached = false;
		callFunction->callFunction.isTSSL = false;
		c.output.add(callFunction);

		if(this->parent->requiresSemicolon(this)) { // if we do not assign/need the value of the function, just pop it
			ts::Instruction* pop = new ts::Instruction();
			pop->type = ts::instruction::POP;
			c.output.add(pop);
		}

		++iterator;
		++iterator;
	}

	int count = 0;
	ts::Instruction* lastInstruction = nullptr;
	for(; iterator != this->elements.end(); ++iterator) {
		AccessElement element = *iterator;

		if(element.token.type == LOCAL_VARIABLE) {
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::LOCAL_ACCESS;
			instruction->localAccess.dimensions = 0;
			instruction->localAccess.hash = hash<string>{}(element.token.lexeme);
			new((void*)&instruction->localAccess.source) string(element.token.lexeme); // TODO move this initialization elsewhere
			instruction->localAccess.stackIndex = -1;

			c.lastAccess = instruction;

			lastInstruction = instruction;
		}
		else if(element.token.type == GLOBAL_VARIABLE) {
			this->parser->error("global variables not supported");
		}
		else if(element.token.type == SYMBOL) {
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::INVALID_INSTRUCTION; // TODO figure out a better way to do this
			instruction->localAccess.dimensions = 0;
			instruction->localAccess.hash = hash<string>{}(element.token.lexeme);
			new((void*)&instruction->localAccess.source) string(element.token.lexeme); // TODO move this initialization elsewhere

			c.lastAccess = instruction;

			lastInstruction = instruction;
		}
		else if(element.isArray) {
			lastInstruction->localAccess.dimensions = ((ArrayStatement*)element.component)->getDimensions();
			c.output.add(element.component->compile(interpreter, scope));
		}
		else if(element.token.type == MEMBER_CHAIN) {
			if(lastInstruction != nullptr) {
				if(lastInstruction->type == ts::instruction::LOCAL_ACCESS && lastInstruction->localAccess.dimensions == 0) {
					lastInstruction->localAccess.stackIndex = scope->allocateVariable(lastInstruction->localAccess.source).stackIndex;
				}
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
		else if(element.component != nullptr && element.component->getType() == PARENT_STATEMENT) {
			c.output.add(element.component->compile(interpreter, scope));
			lastInstruction = nullptr;
		}
		else if(element.component != nullptr && element.component->getType() == CALL_STATEMENT) {
			// at this point, the object should already be on the stack. no need to push it. push the args
			c.output.add(element.component->compile(interpreter, scope));

			// push the amount of arguments we just found
			ts::Instruction* pushArgumentCount = new ts::Instruction();
			pushArgumentCount->type = ts::instruction::PUSH;
			pushArgumentCount->push.entry.type = ts::entry::NUMBER;
			pushArgumentCount->push.entry.setNumber(((CallStatement*)element.component)->getElementCount() + 1);
			c.output.add(pushArgumentCount);

			// compile the call instruction
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::CALL_OBJECT;
			new((void*)&instruction->callObject.name) string(lastInstruction->objectAccess.source); // TODO move this initialization elsewhere

			c.output.add(instruction);

			if(this->parent->requiresSemicolon(this)) { // if we do not assign/need the value of the function, just pop it
				ts::Instruction* pop = new ts::Instruction();
				pop->type = ts::instruction::POP;
				c.output.add(pop);
			}
			
			delete lastInstruction; // this is a bit hacky, but whatever. forget about our last instruction
			lastInstruction = nullptr;
		}
		else if(element.component != nullptr && element.component->getType() == MATH_EXPRESSION) {
			c.output.add(element.component->compile(interpreter, scope));
			lastInstruction = nullptr;
		}
		count++;
	}

	if(lastInstruction != nullptr) {
		if(lastInstruction->type == ts::instruction::LOCAL_ACCESS && lastInstruction->localAccess.dimensions == 0) {
			lastInstruction->localAccess.stackIndex = scope->allocateVariable(lastInstruction->localAccess.source).stackIndex;
		}
		c.output.add(lastInstruction);
	}

	return c;
}

string AccessStatement::getVariableName() {
	return this->elements[0].token.lexeme;
}