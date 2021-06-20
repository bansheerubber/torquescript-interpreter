#pragma once

#include <string>

#include "object.h"
#include "stack.h"

using namespace std;

namespace ts {
	namespace entry {
		enum EntryType {
			INVALID,
			NUMBER,
			STRING,
			OBJECT,
		};
	}
	
	struct Entry {
		entry::EntryType type;
		union {
			double numberData;
			string* stringData;
			Object* objectData;
		};

		Entry();
		Entry(Entry* copy);
		~Entry();
		void setNumber(double value);
		void setString(string &value);
		void setString(string* value);
		void setObject(Object* value);
		void print(int tabs = 0) const;
		const char* typeToString() const;
	};

	void copyEntry(Entry &source, Entry &destination);
}