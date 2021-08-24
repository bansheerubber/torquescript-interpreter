#include "objectReference.h"
#include "object.h"

ObjectReference::ObjectReference(ObjectWrapper* object) {
	this->objectWrapper = object;
	
	if(this->objectWrapper != nullptr) {
		this->objectWrapper->object->addReference(this);
		this->id = this->objectWrapper->object->id;
	}
}

ObjectReference::ObjectReference(ObjectReference* objectReference) {
	this->objectWrapper = objectReference->objectWrapper;

	if(this->objectWrapper != nullptr) {
		this->objectWrapper->object->addReference(this);
		this->id = this->objectWrapper->object->id;
	}
}

ObjectReference::~ObjectReference() {
	if(this->objectWrapper != nullptr) {
		this->objectWrapper->object->removeReference(this);
		this->objectWrapper = nullptr;
	}
}
