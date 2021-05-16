#include "variableContext.h"
#include "instruction.h"
#include "interpreter.h"

using namespace ts;

VariableContext::~VariableContext() {
	
}

void VariableContext::clear() {
	this->variableMap.clear();
}

string VariableContext::computeVariableString(Instruction &instruction, string &variable) {
	string newVariable(variable);

	bool firstDimensionSet = false;
	for(int i = instruction.localAssign.dimensions - 1; i >= 0; i--) {
		if(firstDimensionSet) {
			newVariable += '_';
		}
		
		Entry &entry = this->interpreter->stack[this->interpreter->stackPointer - 1 - i]; // start from top of stack
		// convert double to integer
		if((long)entry.numberData == entry.numberData) {
			newVariable = newVariable + to_string((long)entry.numberData);
		}
		else {
			newVariable = newVariable + to_string(entry.numberData);
		}

		firstDimensionSet = true;
	}
	return newVariable;
}

Entry& VariableContext::getVariableEntry(Instruction &instruction, string &variable) {
	if(instruction.localAssign.dimensions > 0) {
		return this->variableMap[computeVariableString(instruction, variable)];
	}
	return this->variableMap[variable];
}

void VariableContext::setVariableEntry(Instruction &instruction, string &name, Entry &entry) {
	if(instruction.localAssign.dimensions > 0) {
		copyEntry(entry, this->variableMap[computeVariableString(instruction, name)]);
	}
	else {
		copyEntry(entry, this->variableMap[name]); // [] operator automatically creates entries
	}
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto it = this->variableMap.begin(); it != this->variableMap.end(); it++) {
		printf("%s:\n", it->first.c_str());
		it->second.print();
		printf("-------------------------------\n");
	}
}