#include "objectReference.h"
#include "object.h"

ObjectReference::ObjectReference(Object* object) {
	object->addReference(this);
	this->object = object;
}