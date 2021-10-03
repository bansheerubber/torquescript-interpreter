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

			/*
				strings are freed every once in a while depending on what the interpreter is doing.
				- strings are always deleted when their parent entries are popped from the stack
				- strings are deleted from the destination during a `copyEntry` operation
				- strings are overwritten and deleted when using `setString` on an entry with string type
				these happen in some bizzare places due to optimization problems. freeing strings every
				time we did an entry type conversion proved to be too slow while this method is faster.
				i'm trying to keep number operations as quick as possible, and this was the way i found
				to preserve performance
			*/
			char* stringData;
			ObjectReference* objectData;
		};

		Entry();
		Entry(const Entry &entry);
		Entry(double value);
		Entry(char* value);
		Entry(ObjectReference* value);
		~Entry();
		void setNumber(double value);
		void setString(char* value);
		void setString(string value);
		void setObject(ObjectReference* value);
		void erase();
		void print(int tabs = 0) const;
		const char* typeToString() const;

		private:
			void __erase() __attribute__((always_inline));
	};

	void copyEntry(const Entry &source, Entry &destination);
	void greedyCopyEntry(Entry &source, Entry &destination);
	void convertToType(class Interpreter* interpreter, Entry &source, entry::EntryType type);
	void initEntry(class Interpreter* interpreter, Entry* location);
}

namespace std {
	template<> // specialization
	void swap<ts::Entry>(ts::Entry &entry1, ts::Entry &entry2) noexcept;
}
