#pragma once

#include <string>

#include "objectReference.h"

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
			ObjectReference* objectData;
		};

		Entry();
		Entry(const Entry &entry);
		Entry(Entry* copy);
		~Entry();
		void setNumber(double value);
		void setString(string &value);
		void setString(string* value);
		void setObject(ObjectReference* value);
		void print(int tabs = 0) const;
		const char* typeToString() const;
	};

	void copyEntry(const Entry &source, Entry &destination);
	void initEntry(class Interpreter* interpreter, Entry* location);
}