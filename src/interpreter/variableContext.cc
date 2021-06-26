#include "variableContext.h"
#include "instruction.h"
#include "interpreter.h"
#include "entry.h"

#include "entry.h"

using namespace ts;

VariableContext::VariableContext() {

}

VariableContext::VariableContext(Interpreter* interpreter) {
	this->interpreter = interpreter;
}

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
		
		Entry &entry = this->interpreter->stack.array[this->interpreter->stack.head - 1 - i]; // start from top of stack
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

Entry& VariableContext::getVariableEntry(Instruction &instruction, string &name, size_t hash) {
	if(instruction.localAssign.dimensions > 0) {
		string computedString = computeVariableString(instruction, name);
		auto value = this->variableMap.find(computedString);
		if(value == this->variableMap.end()) { // initialize empty string
			this->interpreter->warning("trying to access unassigned variable/property '%s'\n", computedString.c_str());
			
			Entry &entry = this->variableMap[computedString];
			copyEntry(this->interpreter->emptyEntry, entry);
			return entry;
		}
		else {
			return value.value();
		}
	}
	else {
		auto value = this->variableMap.find(name, hash);
		if(value == this->variableMap.end()) { // initialize empty string
			this->interpreter->warning("trying to access unassigned variable/property '%s'\n", name.c_str());
			
			Entry &entry = this->variableMap[name];
			copyEntry(this->interpreter->emptyEntry, entry);
			return entry;
		}
		else {
			return value.value();
		}
	}
}

void VariableContext::setVariableEntry(Instruction &instruction, string &name, size_t hash, Entry &entry) {
	if(instruction.localAssign.dimensions > 0) {
		string computedString = computeVariableString(instruction, name);
		auto value = this->variableMap.find(computedString);
		if(value == this->variableMap.end()) {
			copyEntry(entry, this->variableMap[computedString]);
		}
		else {
			copyEntry(entry, value.value());
		}
	}
	else {
		auto value = this->variableMap.find(name, hash);
		if(value == this->variableMap.end()) {
			copyEntry(entry, this->variableMap[name]);
		}
		else {
			copyEntry(entry, value.value());
		}
	}
}

void VariableContext::setVariableEntry(string &name, Entry &entry) {
	auto value = this->variableMap.find(name);
	if(value == this->variableMap.end()) {
		copyEntry(entry, this->variableMap[name]);
	}
	else {
		copyEntry(entry, value.value());
	}
}

Entry& VariableContext::getVariableEntry(string &name) {
	auto value = this->variableMap.find(name);
	if(value == this->variableMap.end()) { // initialize empty string
		this->interpreter->warning("trying to access unassigned variable/property '%s'\n", name.c_str());
		
		Entry &entry = this->variableMap[name];
		copyEntry(this->interpreter->emptyEntry, entry);
		return entry;
	}
	else {
		return value.value();
	}
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto it = this->variableMap.begin(); it != this->variableMap.end(); it++) {
		printf("\"%s\":\n", it->first.c_str());
		it->second.print(1);
		printf("-------------------------------\n");
	}
}

void VariableContext::printWithTab(int tabs) {
	string space;
	for(int i = 0; i < tabs; i++) {
		space += "   ";
	}
	
	for(auto it = this->variableMap.begin(); it != this->variableMap.end(); it++) {
		printf("%s\"%s\":\n", space.c_str(), it->first.c_str());
		it->second.print(tabs + 1);
	}
}

void initVariableContext(VariableContext* location) {
	
}
