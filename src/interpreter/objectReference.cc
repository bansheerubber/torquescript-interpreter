#include "objectReference.h"
#include "object.h"

ObjectReference::ObjectReference(Object* object) {
	this->object = object;
	
	if(this->object != nullptr) {
		this->object->addReference(this);
		this->id = this->object->id;
	}
}

ObjectReference::ObjectReference(ObjectReference* objectReference) {
	this->object = objectReference->object;

	if(this->object != nullptr) {
		this->object->addReference(this);
		this->id = this->object->id;
	}
}

ObjectReference::~ObjectReference() {
	if(this->object != nullptr) {
		this->object->removeReference(this);
		this->object = nullptr;
	}
}
