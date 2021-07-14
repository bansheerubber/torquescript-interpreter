#include "methodTree.h"

#include "function.h"
#include "packagedFunctionList.h"

using namespace ts;

void ts::initMethodTreePackagedFunctionList(class MethodTree* tree, PackagedFunctionList** list) {
	*list = nullptr;
}

void ts::initMethodTree(MethodTree* self, MethodTree** tree) {
	*tree = nullptr;
}

MethodTreeEntry::MethodTreeEntry(MethodTree* tree) {
	this->list = DynamicArray<PackagedFunctionList*, MethodTree>(tree, 5, initMethodTreePackagedFunctionList, nullptr);
}

MethodTree::MethodTree() {

}

MethodTree::MethodTree(string name) {
	this->name = name;
	this->parents = DynamicArray<MethodTree*, MethodTree>(this, 5, initMethodTree, nullptr);
	this->children = DynamicArray<MethodTree*, MethodTree>(this, 5, initMethodTree, nullptr);
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

void MethodTree::addParent(MethodTree* parent) {
	this->parents[this->parents.head] = parent;
	this->parents.pushed();

	parent->addChild(this);
}

void MethodTree::addChild(MethodTree* child) {
	this->children[this->children.head] = child;
	this->children.pushed();
}
