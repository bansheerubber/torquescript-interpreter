#include "scope.h"

using namespace ts;

BoundVariable& Scope::allocateVariable(string &variableName, bool isArgument) {
	if(this->variables.find(variableName) == this->variables.end()) {
		this->variables[variableName] = (BoundVariable){
			stackIndex: this->stackIndex++,
			name: variableName,
			isArgument: isArgument,
		};
	}
	return this->variables[variableName];
}

ts::InstructionReturn Scope::compileLinkVariables(ts::Interpreter* interpreter) {
	ts::InstructionReturn output;

	for(auto const& [name, value]: this->variables) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::LINK_VARIABLE;
		instruction->linkVariable.stackIndex = value.stackIndex;
		new((void*)&instruction->linkVariable.source) string(name);
		instruction->linkVariable.hash = hash<string>{}(name);

		output.add(instruction);
	}

	return output;
}

size_t Scope::allocatedSize() {
	return this->variables.size();
}
