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
			VariableContext();
			VariableContext(class Interpreter* interpreter);
			~VariableContext();
			
			Entry& getVariableEntry(class Instruction &instruction, string &variable, size_t hash);
			Entry& getVariableEntry(string &name);
			void setVariableEntry(class Instruction &instruction, string &name, size_t hash, Entry &entry);
			void setVariableEntry(string &name, Entry &entry);
			string computeVariableString(class Instruction &instruction, string &variable);
			void print();
			void printWithTab(int tabs);
			void clear();
		
		private:
			class Interpreter* interpreter;
			friend class Interpreter;
			friend class Object;
			robin_map<string, Entry> variableMap;
	};

	void initVariableContext(VariableContext* location);
}