#include "entry.h"

using namespace ts;

Entry::Entry() {
	this->type = entry::INVALID;
	this->numberData = 0.0;
	this->stringData = nullptr;
}

Entry::Entry(Entry* copy) {
	this->type = copy->type;
	this->numberData = 0.0;
	this->stringData = nullptr;
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
	if(this->stringData != nullptr) {
		delete this->stringData;
	}
}

void Entry::setNumber(double value) {
	this->numberData = value;
	this->type = entry::NUMBER;
}

void Entry::setString(string& value) {
	this->stringData = new string(value);
	this->type = entry::STRING;
}

void Entry::print() {
	printf("ENTRY {\n");

	printf("   type: %d,\n", this->type);

	if(this->type == entry::STRING) {
		printf("   data: \"%s\",\n", this->stringData->c_str());
	}
	else {
		printf("   data: %f,\n", this->numberData);
	}

	printf("};\n");
}

void ts::copyEntry(Entry& source, Entry& destination) {
	destination.type = source.type;
	destination.numberData = 0.0;
	destination.stringData = nullptr;
	switch(destination.type) {
		case entry::INVALID: {
			break;
		}
		
		case entry::NUMBER: {
			destination.setNumber(source.numberData);
			break;
		}

		case entry::STRING: {
			destination.setString(*source.stringData);
			break;
		}
	}	
}