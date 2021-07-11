#include "methodTree.h"

using namespace ts;

void ts::initMethodTreePackagedFunctionList(class MethodTree* tree, PackagedFunctionList** list) {
	*list = nullptr;
}

MethodTreeEntry::MethodTreeEntry(MethodTree* tree) {
	this->list = DynamicArray<PackagedFunctionList*, class MethodTree>(tree, 5, initMethodTreePackagedFunctionList, nullptr);
}

MethodTree::MethodTree() {

}

MethodTree::MethodTree(string name) {
	this->name = name;
}

void MethodTree::defineInitialMethod(string name, size_t nameIndex, Function* container) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(nameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[nameIndex] = new MethodTreeEntry(this);
	}
	else {
		entry = this->methodIndexToEntry[nameIndex];
		delete entry->list[0];
	}

	PackagedFunctionList* list = new PackagedFunctionList(name);
	list->addInitialFunction(container);
	entry->list[0] = list;
}

void MethodTree::addPackageMethod(string name, size_t nameIndex, Function* container) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(nameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[nameIndex] = new MethodTreeEntry(this);
	}
	else {
		entry = this->methodIndexToEntry[nameIndex];
	}

	if(entry->list[0] == nullptr) {
		entry->list[0] = new PackagedFunctionList(name);
	}

	entry->list[0]->addPackageFunction(container);
}
