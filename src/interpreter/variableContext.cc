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
		string computedString = computeVariableString(instruction, variable);

		Entry* variableEntry = this->variableMap[computedString];
		if(variableEntry == nullptr) { // initialize empty string
			variableEntry = new Entry();
			this->variableMap[computedString] = variableEntry;
			copyEntry(this->interpreter->emptyEntry, *variableEntry);
		}
		printf("%f\n", variableEntry->numberData);
		return *variableEntry;
	}
	else {
		Entry* variableEntry = this->variableMap[variable];
		if(variableEntry == nullptr) { // initialize empty string
			variableEntry = new Entry();
			this->variableMap[variable] = variableEntry;
			copyEntry(this->interpreter->emptyEntry, *variableEntry);
		}
		return *variableEntry;
	}
}

void VariableContext::setVariableEntry(Instruction &instruction, string &name, Entry &entry) {
	if(instruction.localAssign.dimensions > 0) {
		string computedString = computeVariableString(instruction, name);

		Entry* variableEntry = this->variableMap[computedString];
		if(variableEntry == nullptr) {
			variableEntry = new Entry();
			this->variableMap[computedString] = variableEntry;
		}
		copyEntry(entry, *variableEntry);
	}
	else {
		Entry* variableEntry = this->variableMap[name];
		if(variableEntry == nullptr) {
			variableEntry = new Entry();
			this->variableMap[name] = variableEntry;
		}
		copyEntry(entry, *variableEntry);
	}
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto it = this->variableMap.begin(); it != this->variableMap.end(); it++) {
		printf("%s:\n", it->first.c_str());
		it->second->print();
		printf("-------------------------------\n");
	}
}