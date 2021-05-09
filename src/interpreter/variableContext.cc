#include "variableContext.h"

using namespace ts;

VariableContext::~VariableContext() {
	
}

string computeVariableString(string &variable, variable::Array* array) {
	string newVariable(variable);
	variable::Array* next = array;
	while(next != nullptr) {
		if(next != array) {
			newVariable += '_';
		}
		
		// convert double to integer
		if((long)next->entry->numberData == next->entry->numberData) {
			newVariable = newVariable + to_string((long)next->entry->numberData);
		}
		else {
			newVariable = newVariable + to_string(next->entry->numberData);
		}
		next = next->next;
	}
	return newVariable;
}

Entry& VariableContext::getVariableEntry(string &variable, variable::Array* array) {
	if(array != nullptr) {
		return this->variableMap[computeVariableString(variable, array)];
	}
	return this->variableMap[variable];
}

void VariableContext::setVariableEntry(string &name, variable::Array* array, Entry &entry) {
	if(array != nullptr) {
		copyEntry(entry, this->variableMap[computeVariableString(name, array)]);
	}
	else {
		copyEntry(entry, this->variableMap[name]); // [] operator automatically creates entries
	}
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto &[key, value]: this->variableMap) {
		printf("%s:\n", key.c_str());
		value.print();
		printf("-------------------------------\n");
	}
}