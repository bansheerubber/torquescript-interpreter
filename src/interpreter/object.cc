#include "object.h"
#include "interpreter.h"

Object::Object(ts::Interpreter* interpreter, string nameSpace, int namespaceIndex) {
	this->properties.interpreter = interpreter;
	this->id = interpreter->highestObjectId++;

	string id = "id";
	Entry entry;
	entry.setNumber(this->id);
	this->properties.setVariableEntry(id, entry);

	this->references.reserve(512); // reserve spaces for references, should make objects faster

	this->nameSpace = nameSpace;
	this->namespaceIndex = namespaceIndex;
}

Object::~Object() {
	// go through references and clear them
	for(ObjectReference* reference: this->references) {
		reference->object = nullptr;
	}
}

void Object::addReference(ObjectReference* reference) {
	this->references.push_back(reference);
}
