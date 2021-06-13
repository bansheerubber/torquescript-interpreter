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

void Entry::print() const {
	printf("ENTRY {\n");

	printf("   type: %d,\n", this->type);

	if(this->type == entry::STRING) {
		printf("   data: \"%s\",\n", this->stringData->c_str());
	}
	else if(this->type == entry::NUMBER) {
		printf("   data: %f,\n", this->numberData);
	}
	else {
		printf("   data: %ld,\n", (long)this->objectData);
	}

	printf("};\n");
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