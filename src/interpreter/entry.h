#pragma once

#include <string>

using namespace std;

typedef unsigned int stack_location;
typedef unsigned int relative_stack_location;

namespace ts {
	enum EntryType {
		INVALID_ENTRY,
		NUMBER,
		STRING,
	};
	
	struct Entry {
		EntryType type;
		union {
			int* numberData;
			string* stringData;
		};

		Entry() {
			this->type = INVALID_ENTRY;
			this->numberData = nullptr;
		}

		Entry(Entry* copy) {
			this->type = copy->type;
			switch(this->type) {
				case INVALID_ENTRY: {
					break;
				}
				
				case NUMBER: {
					*this->numberData = *copy->numberData;
					break;
				}

				case STRING: {
					this->stringData = new string(*copy->stringData);
					break;
				}
			}
		}
	};
}