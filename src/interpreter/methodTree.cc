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
	this->hasInitialMethod = false;
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

	entry->hasInitialMethod = true;

	PackagedFunctionList* list = new PackagedFunctionList(name);
	list->addInitialFunction(container);
	entry->list[0] = list;

	for(size_t i = 0; i < this->children.head; i++) {
		this->children[i]->updateMethodTree(name, nameIndex);
	}
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

void MethodTree::updateMethodTree(string methodName, size_t methodNameIndex) {
	vector<PackagedFunctionList*> list = this->buildMethodTreeEntryForParents(methodName, methodNameIndex);
	MethodTreeEntry* entry = this->methodIndexToEntry[methodNameIndex];
	entry->list.head = entry->list[0] == nullptr ? 1 : 0;
	for(PackagedFunctionList* functionList: list) {
		entry->list[entry->list.head] = functionList;
		entry->list.pushed();
	}
}

vector<PackagedFunctionList*> MethodTree::buildMethodTreeEntryForParents(string methodName, size_t methodNameIndex) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(methodNameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[methodNameIndex] = new MethodTreeEntry(this);
	}
	else {
		entry = this->methodIndexToEntry[methodNameIndex];
	}
	
	vector<PackagedFunctionList*> list;
	if(entry->list[0] != nullptr) {
		list.push_back(entry->list[0]);
	}

	for(size_t i = 0; i < this->parents.head; i++) {
		vector<PackagedFunctionList*> inheritedList = this->parents[i]->buildMethodTreeEntryForParents(methodName, methodNameIndex);
		list.insert(list.end(), inheritedList.begin(), inheritedList.end());
	}
	return list;
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
