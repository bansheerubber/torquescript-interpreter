#include "object.h"
#include "interpreter.h"

Object::Object(ts::Interpreter* interpreter) {
	this->properties.interpreter = interpreter;
	this->id = interpreter->highestObjectId++;

	string id = "id";
	Entry entry;
	entry.setNumber(this->id);
	this->properties.setVariableEntry(id, entry);
}