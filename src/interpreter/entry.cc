#include "entry.h"

using namespace ts;

Entry::Entry() {
	this->type = entry::INVALID;
	this->numberData = nullptr;
}

Entry::Entry(Entry* copy) {
	this->type = copy->type;
	switch(this->type) {
		case entry::INVALID: {
			break;
		}
		
		case entry::NUMBER: {
			this->setNumber(*copy->numberData);
			break;
		}

		case entry::STRING: {
			this->stringData = new string(*(copy->stringData));
			break;
		}
	}
}

Entry::~Entry() {
	if(this->numberData != nullptr) {
		delete this->numberData;	
	}
}

void Entry::setNumber(double value) {
	if(this->numberData == nullptr) {
		this->numberData = new double;
	}
	*this->numberData = value;
	this->type = entry::NUMBER;
}

void Entry::setString(string value) {
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
		printf("   data: %f,\n", *this->numberData);
	}

	printf("};\n");
}