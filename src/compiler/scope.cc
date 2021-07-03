#include "scope.h"

using namespace ts;

BoundVariable& Scope::allocateVariable(string &variableName) {
	if(this->variables.find(variableName) == this->variables.end()) {
		this->variables[variableName] = (BoundVariable){
			stackIndex: ++this->stackIndex,
			name: variableName,
		};
	}
	return this->variables[variableName];
}
