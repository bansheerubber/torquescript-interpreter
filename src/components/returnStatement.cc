#include "returnStatement.h"
#include "../interpreter/interpreter.h"

bool ReturnStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == RETURN;
}

ReturnStatement* ReturnStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	ReturnStatement* output = new ReturnStatement(parser);
	output->parent = parent;
	parser->expectToken(RETURN);

	// parse an expression
	if(tokenizer->peekToken().type != SEMICOLON) {
		if(!Component::ShouldParse(output, tokenizer, parser)) {
			parser->error("need valid expression for 'return' statement");
		}
		output->operation = Component::Parse(output, tokenizer, parser);

		parser->expectToken(SEMICOLON);
	}
	else {
		tokenizer->getToken(); // absorb semicolon
	}

	return output;
}

string ReturnStatement::print() {
	if(this->operation != nullptr) {
		return "return " + this->operation->print() + ";";
	}
	else {
		return "return;";
	}
}

string ReturnStatement::printJSON() {
	if(this->operation != nullptr) {
		return "{\"type\":\"RETURN_STATEMENT\",\"operation\":" + this->operation->printJSON() + "}";
	}
	else {
		return "{\"type\":\"RETURN_STATEMENT\"}";
	}
}

ts::InstructionReturn ReturnStatement::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// add a return statement that exits out from our function
	ts::Instruction* returnInstruction = new ts::Instruction();
	returnInstruction->type = ts::instruction::RETURN;
	returnInstruction->functionReturn.hasValue = false;

	if(this->operation != nullptr) {
		output.add(this->operation->compile(interpreter, context));
		returnInstruction->functionReturn.hasValue = true;
	}

	output.add(returnInstruction);
	
	return output;
}
