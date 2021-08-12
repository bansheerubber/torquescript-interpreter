#include "scope.h"

#include "../util/allocateString.h"

using namespace ts;

BoundVariable& Scope::allocateVariable(string &variableName, bool isArgument) {
	if(this->variables.find(toLower(variableName)) == this->variables.end()) {
		this->variables[toLower(variableName)] = (BoundVariable){
			stackIndex: this->stackIndex++,
			name: toLower(variableName),
			isArgument: isArgument,
		};
	}
	return this->variables[toLower(variableName)];
}

ts::InstructionReturn Scope::compileLinkVariables(ts::Interpreter* interpreter) {
	ts::InstructionReturn output;

	for(auto const& [name, value]: this->variables) {
		ts::Instruction* instruction = new ts::Instruction();
		instruction->type = ts::instruction::LINK_VARIABLE;
		instruction->linkVariable.stackIndex = value.stackIndex;
		ALLOCATE_STRING(name, instruction->linkVariable.source);
		instruction->linkVariable.hash = hash<string>{}(name);

		output.add(instruction);
	}

	return output;
}

size_t Scope::allocatedSize() {
	return this->variables.size();
}
