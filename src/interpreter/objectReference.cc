#include "objectReference.h"
#include "object.h"

ObjectReference::ObjectReference(Object* object) {
	this->object = object;
	
	if(object != nullptr) {
		object->addReference(this);
		this->id = object->id;
	}
}

ObjectReference::~ObjectReference() {
	printf("deleted object reference\n");
	if(this->object != nullptr) {
		this->object->removeReference(this);
		this->object = nullptr;
	}
}
