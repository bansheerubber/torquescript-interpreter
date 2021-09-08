#include "variableContext.h"

#include "entry.h"
#include "instruction.h"
#include "interpreter.h"
#include "../util/numberToString.h"
#include "object.h"

using namespace ts;

namespace std {
	template<>
	void swap<VariableContextEntry>(VariableContextEntry &entry1, VariableContextEntry &entry2) noexcept {
		using std::swap;
		swap(entry1.stackIndex, entry2.stackIndex);
		swap(entry1.entry, entry2.entry);
	}
}

VariableContext::VariableContext() {}

VariableContext::VariableContext(Interpreter* interpreter) {
	this->interpreter = interpreter;
}

void VariableContext::clear() {
	this->variableMap.clear();
}

void VariableContext::inherit(VariableContext &parent) {
	this->variableMap = parent.variableMap;
}

string VariableContext::computeVariableString(Instruction &instruction, string &variable) {
	string newVariable(variable);

	bool firstDimensionSet = false;
	for(int i = instruction.localAssign.dimensions - 1; i >= 0; i--) {
		if(firstDimensionSet) {
			newVariable += '_';
		}
		
		Entry &entry = this->interpreter->stack[this->interpreter->stack.head - 1 - i]; // start from top of stack
		if(entry.type == entry::NUMBER) {
			newVariable = newVariable + numberToStdString(entry.numberData);
		}
		else if(entry.type == entry::STRING) {
			newVariable = newVariable + toLower(entry.stringData);
		}
		else if(entry.type == entry::OBJECT) {
			newVariable = newVariable + numberToStdString(entry.objectData->id);
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
			
			VariableContextEntry &entry = this->variableMap[computedString];
			copyEntry(this->interpreter->emptyEntry, entry.entry);
			entry.stackIndex = -1;
			return entry.entry;
		}
		else {
			VariableContextEntry &entry = value.value();
			return entry.entry;
		}
	}
	else {
		auto value = this->variableMap.find(name, hash);
		if(value == this->variableMap.end()) { // initialize empty string
			this->interpreter->warning("trying to access unassigned variable/property '%s'\n", name.c_str());
			
			VariableContextEntry &entry = this->variableMap[name];
			copyEntry(this->interpreter->emptyEntry, entry.entry);
			entry.stackIndex = -1;
			return entry.entry;
		}
		else {
			return value.value().entry;
		}
	}
}

void VariableContext::setVariableEntry(Instruction &instruction, string &name, size_t hash, Entry &entry) {
	if(instruction.localAssign.dimensions > 0) {
		string computedString = computeVariableString(instruction, name);
		auto value = this->variableMap.find(computedString);
		if(value == this->variableMap.end()) { // uninitialized
			VariableContextEntry &variableEntry = this->variableMap[computedString];
			copyEntry(entry, variableEntry.entry);
			variableEntry.stackIndex = -1;
		}
		else {
			VariableContextEntry &variableEntry = value.value();
			copyEntry(entry, variableEntry.entry);
		}
	}
	else {
		auto value = this->variableMap.find(name, hash);
		if(value == this->variableMap.end()) { // uninitialized
			VariableContextEntry &variableEntry = this->variableMap[name];
			copyEntry(entry, variableEntry.entry);
			variableEntry.stackIndex = -1;
		}
		else {
			VariableContextEntry &variableEntry = value.value();
			if(variableEntry.stackIndex < 0) {
				copyEntry(entry, variableEntry.entry);
			}
			else {
				copyEntry(entry, this->interpreter->stack[variableEntry.stackIndex + this->interpreter->stackFramePointer]);
			}
		}
	}
}

void VariableContext::setVariableEntry(string &name, Entry &entry) {
	auto value = this->variableMap.find(name);
	if(value == this->variableMap.end()) { // uninitialized
		VariableContextEntry &variableEntry = this->variableMap[name];
		copyEntry(entry, variableEntry.entry);
		variableEntry.stackIndex = -1;
	}
	else {
		VariableContextEntry &variableEntry = value.value();
		if(variableEntry.stackIndex < 0) {
			copyEntry(entry, variableEntry.entry);
		}
		else {
			copyEntry(entry, this->interpreter->stack[variableEntry.stackIndex + this->interpreter->stackFramePointer]);
		}
	}
}

Entry& VariableContext::getVariableEntry(string &name) {
	auto value = this->variableMap.find(name);
	if(value == this->variableMap.end()) { // initialize empty string
		this->interpreter->warning("trying to access unassigned variable/property '%s'\n", name.c_str());
		
		VariableContextEntry &entry = this->variableMap[name];
		copyEntry(this->interpreter->emptyEntry, entry.entry);
		entry.stackIndex = -1;
		return entry.entry;
	}
	else {
		VariableContextEntry &entry = value.value();
		if(entry.stackIndex < 0) {
			return entry.entry;
		}
		else {
			return this->interpreter->stack[entry.stackIndex + this->interpreter->stackFramePointer];
		}
	}
}

void VariableContext::linkVariable(string &name, size_t hash, int stackIndex) {
	this->variableMap[name].stackIndex = stackIndex;
}

void VariableContext::print() {
	printf("-------------------------------\n");
	for(auto it = this->variableMap.begin(); it != this->variableMap.end(); it++) {
		printf("\"%s\":\n", it->first.c_str());
		it->second.entry.print(1);
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
		it->second.entry.print(tabs + 1);
	}
}

void initVariableContext(VariableContext* location) {
	
}
