#pragma once

#include <unordered_map>
#include "../include/robin-map/include/tsl/robin_map.h"

#include "entry.h"

using namespace std;
using namespace tsl;

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
			
			Entry& getVariableEntry(class Instruction &instruction, string &variable);
			void setVariableEntry(class Instruction &instruction, string &name, Entry &entry);
			string computeVariableString(class Instruction &instruction, string &variable);
			void print();
			void clear();
		
		private:
			class Interpreter* interpreter;
			friend class Interpreter;
			robin_map<string, Entry*> variableMap;
	};
}