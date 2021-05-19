#pragma once

#include <string>

#include "stack.h"

using namespace std;

namespace ts {
	namespace entry {
		enum EntryType {
			INVALID,
			NUMBER,
			STRING,
		};
	}
	
	struct Entry {
		entry::EntryType type;
		union {
			double numberData;
			string* stringData;	
		};

		Entry();
		Entry(Entry* copy);
		~Entry();
		void setNumber(double value);
		void setString(string &value);
		void setString(string* value);
		void print() const;
	};

	void copyEntry(Entry &source, Entry &destination);
}