#include "entry.h"

using namespace ts;

Entry::Entry() {
	this->type = entry::INVALID;
	this->numberData = 0.0;
}

Entry::Entry(Entry* copy) {
	this->type = copy->type;
	switch(this->type) {
		case entry::INVALID: {
			break;
		}
		
		case entry::NUMBER: {
			this->setNumber(copy->numberData);
			break;
		}

		case entry::STRING: {
			this->setString(*copy->stringData);
			break;
		}
	}
}

Entry::~Entry() {
	if(this->type == entry::STRING) {
		if(this->stringData != nullptr) {
			delete this->stringData;
		}
	}
}

void Entry::setNumber(double value) {
	this->type = entry::NUMBER;
	this->numberData = value;
}

void Entry::setString(string &value) {
	this->type = entry::STRING;
	this->stringData = new string(value);
}

void Entry::setString(string* value) {
	// TODO possible wild pointer free
	if(this->type == entry::STRING && this->stringData != nullptr) { // delete old string data
		delete this->stringData;
	}
	
	this->type = entry::STRING;
	this->stringData = value;
}

void Entry::setObject(Object* object) {
	this->type = entry::OBJECT;
	this->objectData = object;
}

## entry_debug.py

const char* Entry::typeToString() const {
	return EntryTypeDebug[this->type];
}

void Entry::print(int tabs) const {
	string space;
	for(int i = 0; i < tabs; i++) {
		space += "   ";
	}
	printf("%sENTRY {\n", space.c_str());

	printf("%s   type: %s,\n", space.c_str(), this->typeToString());

	if(this->type == entry::STRING) {
		printf("%s   data: \"%s\",\n", space.c_str(), this->stringData->c_str());
	}
	else if(this->type == entry::NUMBER) {
		printf("%s   data: %f,\n", space.c_str(), this->numberData);
	}
	else if(this->type == entry::OBJECT) {
		printf("%s   data: 0x%lX,\n", space.c_str(), (long)this->objectData);
		printf("%s   variables:\n", space.c_str());
		this->objectData->properties.printWithTab(2 + tabs);
	}
	else {
		printf("%s   data: no data,\n", space.c_str());
	}

	printf("%s};\n", space.c_str());
}

void ts::copyEntry(Entry &source, Entry &destination) {
	destination.type = source.type;
	switch(destination.type) {
		case entry::INVALID: {
			break;
		}
		
		case entry::NUMBER: {
			destination.numberData = source.numberData;
			break;
		}

		case entry::STRING: {
			destination.stringData = new string(*source.stringData);
			break;
		}

		case entry::OBJECT: {
			destination.objectData = source.objectData;
			break;
		}
	}
}