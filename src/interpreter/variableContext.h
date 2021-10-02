#pragma once

#include "entry.h"
#include "../include/robin-map/include/tsl/robin_map.h"

using namespace std;
using namespace tsl;

namespace ts {
	struct VariableContextEntry {
		Entry entry;
		int stackIndex;
	};
	
	class VariableContext {
		friend class Interpreter;
		friend class Object;
		friend void initFunctionFrame(Interpreter* interpreter, class FunctionFrame* frame);
		
		public:
			VariableContext();
			VariableContext(class Interpreter* interpreter);
			
			Entry& getVariableEntry(class Instruction &instruction, string &variable, size_t hash);
			Entry& getVariableEntry(string &name);
			void setVariableEntry(class Instruction &instruction, string &name, size_t hash, Entry &entry, bool greedy);
			void setVariableEntry(string &name, Entry &entry);
			string computeVariableString(class Instruction &instruction, string &variable);
			void linkVariable(string &name, size_t hash, int stackIndex);
			void print();
			void printWithTab(int tabs);
			void clear();
			void inherit(VariableContext &parent);
		
		private:
			class Interpreter* interpreter;
			robin_map<string, VariableContextEntry> variableMap;
	};

	void initVariableContext(VariableContext* location);
}

namespace std {
	template<> // specialization
	void swap<ts::VariableContextEntry>(ts::VariableContextEntry &entry1, ts::VariableContextEntry &entry2) noexcept;
}