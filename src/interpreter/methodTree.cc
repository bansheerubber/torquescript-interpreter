#include "methodTree.h"

#include "function.h"
#include "packagedFunctionList.h"

using namespace ts;

void ts::initMethodTreePackagedFunctionList(MethodTreeEntry* tree, PackagedFunctionList** list) {
	*list = nullptr;
}

void ts::initMethodTree(MethodTree* self, MethodTree** tree) {
	*tree = nullptr;
}

MethodTreeEntry::MethodTreeEntry(MethodTree* tree, string name) {
	this->name = name;
	this->list[0] = new PackagedFunctionList(name, tree->name);
	this->list.pushed();
	this->hasInitialMethod = false;
}

MethodTreeEntry::~MethodTreeEntry() {
	delete this->list[0]; // only delete our initial method
}

MethodTree::MethodTree() {

}

MethodTree::MethodTree(string name, size_t index) {
	this->name = name;
	this->index = index;
}

MethodTree::~MethodTree() {
	for(const auto& it: this->methodIndexToEntry) {
		delete it.second;
	}
}

string MethodTree::GetComplexNamespace(
	string name1,
	string name2,
	string name3,
	string name4,
	string name5
) {
	string names[] = {
		name1,
		name2,
		name3,
		name4,
		name5
	};

	string output; 
	size_t count = 0;
	for(size_t i = 0; i < 5; i++) {
		if(names[i].length() != 0) {
			output += names[i] + "->";
			count++;
		}
	}
	output.pop_back();
	output.pop_back();

	if(count != 1) { // designate this namespace as a special one that was created at runtime
		output = "$" + output;
	}
	
	return output;
}

void MethodTree::defineInitialMethod(string name, size_t nameIndex, Function* container) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(nameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[nameIndex] = new MethodTreeEntry(this, name);
	}
	else {
		entry = this->methodIndexToEntry[nameIndex];
	}

	PackagedFunctionList* list = entry->list[0];

	bool hadInitialMethod = entry->hasInitialMethod;
	entry->hasInitialMethod = true;
	list->defineInitialFunction(container);

	if(!hadInitialMethod) {
		this->updateMethodTree(name, nameIndex);

		for(size_t i = 0; i < this->children.head; i++) {
			this->children[i]->updateMethodTree(name, nameIndex);
		}
	}
}

void MethodTree::addPackageMethod(string name, size_t nameIndex, Function* container) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(nameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[nameIndex] = new MethodTreeEntry(this, name);
	}
	else {
		entry = this->methodIndexToEntry[nameIndex];
	}

	entry->list[0]->addPackageFunction(container);
}

void MethodTree::updateMethodTree(string methodName, size_t methodNameIndex) {
	vector<PackagedFunctionList*> list = this->buildMethodTreeEntryForParents(methodName, methodNameIndex, false);
	MethodTreeEntry* entry = this->methodIndexToEntry[methodNameIndex];
	entry->list.head = 1;
	for(PackagedFunctionList* functionList: list) {
		entry->list[entry->list.head] = functionList;
		entry->list.pushed();
	}
}

vector<PackagedFunctionList*> MethodTree::buildMethodTreeEntryForParents(string methodName, size_t methodNameIndex, bool addInitial) {
	MethodTreeEntry* entry;
	if(this->methodIndexToEntry.find(methodNameIndex) == this->methodIndexToEntry.end()) {
		entry = this->methodIndexToEntry[methodNameIndex] = new MethodTreeEntry(this, methodName);
	}
	else {
		entry = this->methodIndexToEntry[methodNameIndex];
	}
	
	vector<PackagedFunctionList*> list;
	if(addInitial && entry->hasInitialMethod) {
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

	for(auto &[index, entry]: this->methodIndexToEntry) {
		child->updateMethodTree(entry->name, index);
	}
}

void MethodTree::print() {
	printf("%s methods:\n", this->name.c_str());
	for(auto &[_, entry]: this->methodIndexToEntry) {
		printf("   %s:\n", entry->name.c_str());
		
		if(!entry->hasInitialMethod) {
			printf("      0: no initial method\n");
		}

		for(size_t i = entry->hasInitialMethod ? 0 : 1; i < entry->list.head; i++) {
			PackagedFunctionList* list = entry->list[i];
			printf("      %ld: %s::%s\n", i, list->functionNamespace.c_str(), list->functionName.c_str());
		}
	}
	printf("\n");
}
