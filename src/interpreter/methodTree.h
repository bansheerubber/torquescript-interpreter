#pragma once

#include <string>

#include "dynamicArray.h"
#include "../include/robin-map/include/tsl/robin_map.h"

using namespace tsl;
using namespace std;

namespace ts {
	void initMethodTreePackagedFunctionList(class MethodTree* tree, class PackagedFunctionList** list);
	void initMethodTree(class MethodTree* self, class MethodTree** tree);
	
	struct MethodTreeEntry {
		DynamicArray<class PackagedFunctionList*, class MethodTree> list;
		bool hasInitialMethod;
		string name;

		MethodTreeEntry() {
			this->hasInitialMethod = false;
		}
		MethodTreeEntry(class MethodTree* tree, string name);
	};
	
	class MethodTree {
		public:
			MethodTree();
			MethodTree(string name);

			void defineInitialMethod(string name, size_t nameIndex, class Function* container);
			void addPackageMethod(string name, size_t nameIndex, class Function* container);

			// add a parent for this method tree, order matters
			void addParent(MethodTree* parent);

			// adds a child to this method tree, order doesn't matter
			void addChild(MethodTree* child);

			void updateMethodTree(string methodName, size_t methodNameIndex);

			// each method gets its own index assigned to it by the interpreter. the method's index is based on its name,
			// so we can have a method with the same name that is defined in several unrelated namespaces but that method
			// still gets the same index as the rest of the methods with the same name
			robin_map<size_t, MethodTreeEntry*> methodIndexToEntry;
			string name;
		
		private:
			vector<class PackagedFunctionList*> buildMethodTreeEntryForParents(string methodName, size_t methodNameIndex);
			DynamicArray<MethodTree*, MethodTree> parents;
			DynamicArray<MethodTree*, MethodTree> children;
	};
}
