#include "variableContext.h"

using namespace ts;

VariableContext::~VariableContext() {
	// delete all the entry values from the map
	for(auto const&[key, value]: this->variableMap) {
		delete value;
	}
}

Entry* VariableContext::getVariableEntry(string variable) {
	return this->variableMap[variable];
}

void VariableContext::setVariableEntry(string name, Entry* entry) {
	this->variableMap[name] = new Entry(entry);
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto const&[key, value]: this->variableMap) {
		printf("%s:\n", key.c_str());
		value->print();
		printf("-------------------------------\n");
	}
}