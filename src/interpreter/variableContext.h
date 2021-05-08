#pragma once

#include <unordered_map>

#include "entry.h"

using namespace std;

namespace ts {
	class VariableContext {
		public:
			~VariableContext();
			
			Entry& getVariableEntry(string variable);
			void setVariableEntry(string name, Entry& entry);
			void print();
		
		private:
			unordered_map<string, Entry> variableMap;
	};
}