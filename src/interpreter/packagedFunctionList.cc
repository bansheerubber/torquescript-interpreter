#include "packagedFunctionList.h"

using namespace ts;

// for the dynamic array
void ts::initPackagedFunctionListFunction(PackagedFunctionList* hierarchy, Function** function) {
	*function = nullptr;
}

PackagedFunctionList::PackagedFunctionList() {
	
}

PackagedFunctionList::PackagedFunctionList(string functionName, string functionNamespace) {
	this->functionName = functionName;
	this->functionNamespace = functionNamespace;
}

PackagedFunctionList::~PackagedFunctionList() {
	for(size_t i = 0; i < this->functions.head; i++) {
		if(this->functions[i] != nullptr) {
			delete this->functions[i];
			this->functions[i] = nullptr;
		}
	}

	this->functions.head = 0;
}

void PackagedFunctionList::defineInitialFunction(Function* function) {
	if(this->functions[0] != nullptr) {
		delete this->functions[0];
	}
	
	this->functions[0] = function;
	this->functions.pushed();

	this->findValidFunction();
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
	for(int i = this->functions.head - 1; i >= 0; i--) {
		if(this->functions[i] != nullptr && this->functions[i]->isActive) {
			this->topValidIndex = i;
			break;
		}
	}
}
