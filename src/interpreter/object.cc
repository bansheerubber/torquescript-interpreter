#include "object.h"

#include "interpreter.h"
#include "methodTree.h"

ObjectWrapper* ts::CreateObject(class ts::Interpreter* interpreter, string nameSpace, string inheritedName, MethodTree* methodTree, void* data) {
	Object* object = new Object(interpreter, nameSpace, inheritedName, methodTree);
	ObjectWrapper* wrapper = new ObjectWrapper(object, data);
	interpreter->objects[object->id] = wrapper;
	return wrapper;
}

Object::Object(ts::Interpreter* interpreter, string nameSpace, string inheritedName, MethodTree* methodTree) {
	this->properties.interpreter = interpreter;

	if(inheritedName.length() != 0) {
		// TODO hash
		auto objectIterator = interpreter->stringToObject.find(inheritedName);
		if(objectIterator != interpreter->stringToObject.end()) {
			this->inherit(objectIterator->second->object);
		}
	}

	this->id = interpreter->highestObjectId++;

	string id = "id";
	Entry entry;
	entry.setNumber(this->id);
	this->properties.setVariableEntry(id, entry);

	this->nameSpace = nameSpace;
	this->methodTree = methodTree;
}

Object::~Object() {
	ObjectReference* reference = this->list;
	while(reference != nullptr) {
		reference->objectWrapper = nullptr;
		reference = reference->next;
	}
	
	this->properties.interpreter->deleteObjectName(this->name);
	this->properties.interpreter->objects.erase(this->id);
}

void Object::inherit(Object* parent) {
	this->properties.inherit(parent->properties);
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

void Object::setName(string &name) {
	this->name = name;
}
