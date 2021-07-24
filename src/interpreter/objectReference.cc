#include "objectReference.h"
#include "object.h"

ObjectReference::ObjectReference(Object* object) {
	object->addReference(this);
	this->object = object;
}

ObjectReference::~ObjectReference() {
	printf("deleted object reference\n");
	this->object->removeReference(this);
	this->object = nullptr;
}
