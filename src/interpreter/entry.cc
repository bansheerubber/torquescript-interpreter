#include "entry.h"

#include "../util/cloneString.h"
#include "../util/getEmptyString.h"
#include "interpreter.h"
#include "../util/isInteger.h"
#include "../util/numberToString.h"
#include "object.h"
#include "../util/stringToNumber.h"

using namespace ts;

Entry::Entry() {
	this->type = entry::INVALID;
	this->stringData = nullptr;
}

Entry::Entry(const Entry &source) {
	this->type = entry::INVALID;
	copyEntry(source, *this);
}

Entry::Entry(double value) {
	this->type = entry::NUMBER;
	this->numberData = value;
}

Entry::Entry(char* value) {
	this->type = entry::STRING;
	this->stringData = value;
}

Entry::Entry(ObjectReference* value) {
	this->type = entry::OBJECT;
	this->objectData = value;
}

Entry::~Entry() {
	if(this->type == entry::STRING && this->stringData != nullptr) {
		delete[] this->stringData;
		this->stringData = nullptr;
	}

	if(this->type == entry::OBJECT && this->objectData != nullptr) {
		delete this->objectData;
		this->objectData = nullptr;
	}
}

namespace std {
	template<>
	void swap<Entry>(Entry &entry1, Entry &entry2) noexcept {
		using std::swap;
		swap(entry1.type, entry2.type);
		swap(entry1.stringData, entry2.stringData);
	}
}

void Entry::setNumber(double value) {
	this->__erase();
	this->type = entry::NUMBER;
	this->numberData = value;
}

void Entry::setString(char* value) {
	this->__erase();
	this->type = entry::STRING;
	this->stringData = value;
}

void Entry::setString(string value) {
	this->__erase();
	this->type = entry::STRING;
	this->stringData = stringToChars(value);
}

void Entry::setObject(ObjectReference* object) {
	this->__erase();
	this->type = entry::OBJECT;
	this->objectData = object;
}

void Entry::erase() {
	this->__erase();
}

void Entry::__erase() {
	if(this->type == entry::STRING && this->stringData != nullptr) {
		delete[] this->stringData;
		this->stringData = nullptr;
	}

	if(this->type == entry::OBJECT && this->objectData != nullptr) {
		delete this->objectData;
		this->objectData = nullptr;
	}
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
		printf("%s   data: \"%s\",\n", space.c_str(), this->stringData);
	}
	else if(this->type == entry::NUMBER) {
		printf("%s   data: %f,\n", space.c_str(), this->numberData);
	}
	else if(this->type == entry::OBJECT) {
		printf("%s   data: 0x%lX,\n", space.c_str(), (long)this->objectData->objectWrapper);
		if(this->objectData->objectWrapper != nullptr) {
			printf("%s   variables:\n", space.c_str());
			this->objectData->objectWrapper->object->properties.printWithTab(2 + tabs);
		}
	}
	else {
		printf("%s   data: no data,\n", space.c_str());
	}

	printf("%s};\n", space.c_str());
}

void ts::copyEntry(const Entry &source, Entry &destination) {
	destination.erase();
	
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
			destination.stringData = cloneString(source.stringData);
			break;
		}

		case entry::OBJECT: {
			destination.objectData = new ObjectReference(source.objectData);
			break;
		}
	}
}

void ts::greedyCopyEntry(Entry &source, Entry &destination) {
	destination.erase();
	
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
			destination.stringData = source.stringData;
			source.stringData = nullptr;
			break;
		}

		case entry::OBJECT: {
			destination.objectData = new ObjectReference(source.objectData);
			break;
		}
	}
}

void ts::convertToType(Interpreter* interpreter, Entry &source, entry::EntryType type) {
	if(source.type == type) {
		return;
	}

	switch(type) {
		case entry::NUMBER: {
			## type_conversion.py source source.numberData STRING_OBJECT NUMBER "" interpreter
			break;
		}

		case entry::OBJECT: {
			ObjectWrapper* objectWrapper = nullptr;
			## type_conversion.py source objectWrapper NUMBER_STRING OBJECT "" interpreter
			source.objectData = new ObjectReference(objectWrapper);
			break;
		}

		case entry::STRING: {
			## type_conversion.py source source.stringData NUMBER_OBJECT STRING "" interpreter
			break;
		}
	}

	source.type = type;
}

void ts::initEntry(class Interpreter* interpreter, Entry* location) {;
	new((void*)location) Entry();
}