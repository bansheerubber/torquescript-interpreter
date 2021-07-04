#include "function.h"

using namespace ts;

Function::Function(Instruction* head, size_t argumentCount, string functionName, string namespaceName) : InstructionContainer(head) {
	this->functionName = functionName;
	this->namespaceName = namespaceName;
	this->argumentCount = argumentCount;
}

Function::Function(sl::Function* tsslFunction) {
	this->isTSSL = true;
	this->function = tsslFunction;
	this->functionName = tsslFunction->name;
	this->namespaceName = "";
}