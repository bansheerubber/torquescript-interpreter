#include "functionDeclaration.h"
#include "../interpreter/interpreter.h"
#include "accessStatement.h"

bool FunctionDeclaration::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == FUNCTION;
}

FunctionDeclaration* FunctionDeclaration::Parse(Body* parent, Tokenizer* tokenizer, Parser* parser) {
	if(parent->getType() != SOURCE_FILE && parent->getType() != PACKAGE_DECLARATION) {
		parser->error("cannot declare scoped functions");
	}
	
	FunctionDeclaration* output = new FunctionDeclaration(parser);
	output->parent = parent;
	parser->expectToken(FUNCTION);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid function name");
	}
	output->name1 = Symbol::Parse(output, tokenizer, parser);

	if(tokenizer->peekToken().type == NAMESPACE) {
		parser->expectToken(NAMESPACE);

		// parse a symbol
		if(!Symbol::ShouldParse(tokenizer, parser)) {
			parser->error("invalid function name");
		}
		output->name2 = Symbol::Parse(output, tokenizer, parser);
	}

	// parse the argument list
	if(!CallStatement::ShouldParse(tokenizer, parser)) {
		parser->error("expected arguments for function");
	}
	output->args = CallStatement::Parse(output, tokenizer, parser);

	// find errors in args
	auto it = output->args->getElements();
	for (; it.first != it.second; it.first++) {
		Component* component = (*(it.first)).component;
		if(
			component != nullptr
			&& (
				component->getType() != ACCESS_STATEMENT
				|| !((AccessStatement*)component)->isLocalVariable()
				|| ((AccessStatement*)component)->chainSize() != 1
			)
		) {
			parser->error("unexpected argument '%s' for function declaration", component->print().c_str());
		}
	}

	parser->expectToken(LEFT_BRACKET);
	
	Component::ParseBody(output, tokenizer, parser);

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string FunctionDeclaration::print() {
	string output = "function " + this->name1->print() + this->args->print() + this->parser->space + "{" + this->parser->newLine;
	if(this->name2 != nullptr) {
		output = "function " + this->name1->print() + "::" + this->name2->print() + this->args->print() + this->parser->space + "{" + this->parser->newLine;
	}

	output += this->printBody();
	output += "}" + this->parser->newLine;
	return output;
}

string FunctionDeclaration::printJSON() {
	if(this->name2 == nullptr) {
		return "{\"type\":\"FUNCTIONAL_DECLARATION\",\"name1\":" + this->name1->printJSON() + ",\"arguments\":" + this->args->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
	}
	else {
		return "{\"type\":\"FUNCTIONAL_DECLARATION\",\"name1\":" + this->name1->printJSON() + ",\"name2\":" + this->name2->printJSON() + ",\"arguments\":" + this->args->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
	}
}

ts::InstructionReturn FunctionDeclaration::compile(ts::Interpreter* interpreter) {
	ts::InstructionReturn output;

	// loop through the arguments and assign them from values on the stack
	auto it = this->args->getElements();
	int count = 0;
	int argumentCount = this->args->getElementCount();
	for (; it.first != it.second; it.first++) {
		AccessStatement* component = (AccessStatement*)(*(it.first)).component; // we know these are local variables
		if(component != nullptr) {
			// assign a value from the stack to this variable	
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = ts::instruction::ARGUMENT_ASSIGN;
			new((void*)&instruction->argumentAssign.destination) string(component->getVariableName());
			instruction->argumentAssign.hash = hash<string>{}(component->getVariableName());
			instruction->argumentAssign.dimensions = 0;
			instruction->argumentAssign.offset = argumentCount - count;
			instruction->argumentAssign.argc = argumentCount;
			count++;
			output.add(instruction);
		}
	}

	// tell the interpreter to pop values from the stack that were pushed as arguments
	ts::Instruction* instruction = new ts::Instruction();
	instruction->type = ts::instruction::POP_ARGUMENTS;
	output.add(instruction);

	// compile the body of the function
	for(Component* component: this->children) {
		output.add(component->compile(interpreter));
	}

	// push the empty value if we do not actually use a return statement from earlier in the function body
	ts::Instruction* pushEmpty = new ts::Instruction();
	pushEmpty->type = ts::instruction::PUSH;
	copyEntry(interpreter->emptyEntry, pushEmpty->push.entry);
	output.add(pushEmpty);

	// add a return statement that exits out from our function
	ts::Instruction* returnInstruction = new ts::Instruction();
	returnInstruction->type = ts::instruction::RETURN;	
	output.add(returnInstruction);

	if(this->name2 != nullptr) {
		string nameSpace = this->name1->print();
		string name = this->name2->print();
		interpreter->addFunction(nameSpace, name, output);
	}
	else {
		string name = this->name1->print();
		interpreter->addFunction(name, output); // tell the interpreter to add a function under our name
	}

	return {}; // do not output anything to the body, functions are stored elsewhere
}
