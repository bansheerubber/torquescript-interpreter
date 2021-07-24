#include "object.h"
#include "interpreter.h"

Object::Object(ts::Interpreter* interpreter, string nameSpace, size_t namespaceIndex) {
	this->properties.interpreter = interpreter;
	this->id = interpreter->highestObjectId++;

	string id = "id";
	Entry entry;
	entry.setNumber(this->id);
	this->properties.setVariableEntry(id, entry);

	this->nameSpace = nameSpace;
	this->namespaceIndex = namespaceIndex;
}

Object::~Object() {
	ObjectReference* reference = this->list;
	while(reference != nullptr) {
		reference->object = nullptr;
		reference = reference->next;
	}
}

void Object::addReference(ObjectReference* reference) {
	if(this->list == nullptr) {
		this->list = reference;
	}
	else {
		this->top->next = reference;
		reference->previous = this->top;
	}

	this->top = reference;
}

void Object::removeReference(ObjectReference* reference) {
	if(this->list == reference && this->top == reference) { // clear the list if we only have one reference
		this->list = nullptr;
	}
	else if(this->list == reference) {
		this->list = reference->next;
		reference->next->previous = nullptr;
	}
	else if(this->top == reference) {
		this->top = reference->previous;
		reference->previous->next = nullptr;
	}
	else {
		reference->previous->next = reference->next;
		reference->next->previous = reference->previous;
	}
}
