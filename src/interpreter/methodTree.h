#pragma once

#include <string>

#include "../util/dynamicArray.h"
#include "../include/robin-map/include/tsl/robin_map.h"

using namespace tsl;
using namespace std;

namespace ts {
	void initMethodTreePackagedFunctionList(struct MethodTreeEntry* tree, class PackagedFunctionList** list);
	void initMethodTree(class MethodTree* self, class MethodTree** tree);
	
	struct MethodTreeEntry {
		DynamicArray<class PackagedFunctionList*, MethodTreeEntry> list = DynamicArray<PackagedFunctionList*, MethodTreeEntry>(this, 18, initMethodTreePackagedFunctionList, nullptr);
		bool hasInitialMethod;
		string name;

		MethodTreeEntry() {
			this->hasInitialMethod = false;
		}
		MethodTreeEntry(class MethodTree* tree, string name);
		~MethodTreeEntry();
	};
	
	class MethodTree {
		public:
			MethodTree();
			MethodTree(string name, size_t index);
			~MethodTree();

			static string GetComplexNamespace(
				string name1,
				string name2 = string(),
				string name3 = string(),
				string name4 = string(),
				string name5 = string()
			);

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

			void print();

			size_t index;
		
		private:
			vector<class PackagedFunctionList*> buildMethodTreeEntryForParents(string methodName, size_t methodNameIndex, bool addInitial = true);
			DynamicArray<MethodTree*, MethodTree> parents = DynamicArray<MethodTree*, MethodTree>(this, 17, initMethodTree, nullptr);
			DynamicArray<MethodTree*, MethodTree> children = DynamicArray<MethodTree*, MethodTree>(this, 16, initMethodTree, nullptr);
	};
}
