#include "variableContext.h"

using namespace ts;

VariableContext::~VariableContext() {
	
}

Entry& VariableContext::getVariableEntry(string variable) {
	return this->variableMap[variable];
}

void VariableContext::setVariableEntry(string name, Entry& entry) {
	copyEntry(entry, this->variableMap[name]); // [] operator automatically creates entries
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto &[key, value]: this->variableMap) {
		printf("%s:\n", key.c_str());
		value.print();
		printf("-------------------------------\n");
	}
}