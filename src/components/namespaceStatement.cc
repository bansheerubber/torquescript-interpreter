#include "namespaceStatement.h"
#include "../interpreter/interpreter.h"

bool NamespaceStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		tokenizer->peekToken().type == PARENT
		|| (
			tokenizer->peekToken().type == SYMBOL
			&& tokenizer->peekToken(1).type == NAMESPACE
		)
	);
}

NamespaceStatement* NamespaceStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NamespaceStatement* output = new NamespaceStatement(parser);
	output->parent = parent;

	if(tokenizer->peekToken().type == PARENT) {
		parser->expectToken(PARENT);
	}
	else {
		// parse a symbol
		if(!Symbol::ShouldParse(tokenizer, parser)) {
			parser->error("invalid namespace operation");
		}
		output->name = Symbol::Parse(output, tokenizer, parser);	
	}

	parser->expectToken(NAMESPACE);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid namespace operation");
	}
	output->operation = Symbol::Parse(output, tokenizer, parser);

	// parse call statement
	if(!CallStatement::ShouldParse(tokenizer, parser)) {
		parser->error("invalid namespace operation");
	}
	output->call = CallStatement::Parse(parent, tokenizer, parser);

	return output;
}

string NamespaceStatement::print() {
	string output = "Parent::" + this->operation->print() + this->call->print();
	if(this->name != nullptr) {
		output = this->name->print() + "::" + this->operation->print() + this->call->print();
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}

	return output;
}

string NamespaceStatement::printJSON() {
	if(this->name != nullptr) {
		return "{\"type\":\"NAMESPACE_STATEMENT\",\"name\":" + this->name->printJSON() + ",\"operation\":" + this->operation->printJSON() + ",\"call\":" + this->call->printJSON() + "}";
	}
	else {
		return "{\"type\":\"NAMESPACE_STATEMENT\",\"name\":\"Parent\",\"operation\":" + this->operation->printJSON() + ",\"call\":" + this->call->printJSON() + "}";
	}
}

ts::InstructionReturn NamespaceStatement::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	ts::InstructionReturn output;
	
	if(this->name != nullptr) {
		output.add(this->call->compile(interpreter, scope)); // push arguments

		// push the amount of arguments we just found
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::PUSH;
		instruction->push.entry = ts::Entry();
		instruction->push.entry.type = ts::entry::NUMBER;
		instruction->push.entry.setNumber(((CallStatement*)this->call)->getElementCount());
		output.add(instruction);

		// build call instruction
		ts::Instruction* callFunction = new ts::Instruction();
		callFunction->type = ts::instruction::CALL_FUNCTION;
		new((void*)&callFunction->callFunction.name) string(this->operation->print()); // TODO move this initialization elsewhere
		new((void*)&callFunction->callFunction.nameSpace) string(this->name->print()); // TODO move this initialization elsewhere
		callFunction->callFunction.cachedIndex = 0;
		callFunction->callFunction.cachedNamespaceIndex = 0;
		callFunction->callFunction.isCached = false;
		callFunction->callFunction.isNamespaceCached = false;
		callFunction->callFunction.isTSSL = false;
		output.add(callFunction);

		if(this->parent->requiresSemicolon(this)) { // if we do not assign/need the value of the function, just pop it
			ts::Instruction* pop = new ts::Instruction();
			pop->type = ts::instruction::POP;
			output.add(pop);
		}
	}
	else {
		printf("parent not supported\n");
		exit(1);
	}
	
	return output;
}
