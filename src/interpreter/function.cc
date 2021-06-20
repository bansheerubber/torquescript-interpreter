#include "function.h"

using namespace ts;

Function::Function(Instruction* head, string functionName, string namespaceName) : InstructionContainer(head) {
	this->functionName = functionName;
	this->namespaceName = namespaceName;
}

Function::Function(sl::Function* tsslFunction) {
	this->isTSSL = true;
	this->function = tsslFunction;
	this->functionName = tsslFunction->name;
	this->namespaceName = "";
}