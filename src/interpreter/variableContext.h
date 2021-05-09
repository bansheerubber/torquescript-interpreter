#pragma once

#include <unordered_map>

#include "entry.h"

using namespace std;

namespace ts {
	namespace variable {
		struct Array {
			Entry* entry;
			Array* next;

			Array() {
				this->entry = nullptr;
				this->next = nullptr;
			}

			Array(Entry* entry) {
				this->entry = entry;
				this->next = nullptr;
			}
		};
	}
	
	class VariableContext {
		public:
			~VariableContext();
			
			Entry& getVariableEntry(string &variable, variable::Array* array);
			void setVariableEntry(string &name, variable::Array* array, Entry &entry);
			void print();
		
		private:
			unordered_map<string, Entry> variableMap;
	};
}