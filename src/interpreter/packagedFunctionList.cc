#include "packagedFunctionList.h"

using namespace ts;

// for the dynamic array
void initFunction(PackagedFunctionList* hierarchy, Function** function) {
	*function = nullptr;
}

PackagedFunctionList::PackagedFunctionList() {
	
}

PackagedFunctionList::PackagedFunctionList(string functionName, string functionNamespace) {
	this->functionName = functionName;
	this->functionNamespace = functionNamespace;
	this->functions = DynamicArray<Function*, PackagedFunctionList>(this, PACKAGED_FUNCTION_LIST_START_SIZE, initFunction, nullptr);
}

void PackagedFunctionList::addInitialFunction(Function* function) {
	if(this->functions.head != 0) {
		printError("cannot define initial function twice");
		exit(1);	
	}
	
	this->functions[0] = function;
	this->topValidIndex = 0;
	this->functions.pushed();
}

void PackagedFunctionList::addPackageFunction(Function* function) {
	if(this->functions.head == 0) {
		this->functions.head = 1;
	}
	
	this->functions[this->functions.head] = function;
	this->topValidIndex = this->functions.head;

	this->functions.pushed();
}

// deactiavtes a function in the list
void PackagedFunctionList::deactivateFunction(Function* function) {
	function->isActive = false;
	this->findValidFunction();
}

void PackagedFunctionList::activateFunction(Function* function) {
	function->isActive = true;
	this->findValidFunction();
}

int PackagedFunctionList::getNextValidIndex(int currentIndex) {
	for(int i = currentIndex - 1; i >= 0; i--) {
		if(this->functions[i] != nullptr && this->functions[i]->isActive) {
			return i;
		}
	}

	return -1;
}

void PackagedFunctionList::findValidFunction() {
	// find top-most valid index
	for(size_t i = this->functions.head - 1; i >= 0; i--) {
		if(this->functions[i] != nullptr && this->functions[i]->isActive) {
			this->topValidIndex = i;
			break;
		}
	}
}
