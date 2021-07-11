#pragma once

#include <string>

#include "dynamicArray.h"
#include "function.h"
#include "packagedFunctionList.h"
#include "../include/robin-map/include/tsl/robin_map.h"

using namespace tsl;
using namespace std;

namespace ts {
	void initMethodTreePackagedFunctionList(class MethodTree* tree, PackagedFunctionList** list);
	
	struct MethodTreeEntry {
		DynamicArray<PackagedFunctionList*, class MethodTree> list;

		MethodTreeEntry() {}
		MethodTreeEntry(class MethodTree* tree);
	};
	
	class MethodTree {
		public:
			MethodTree();
			MethodTree(string name);

			void defineInitialMethod(string name, size_t nameIndex, Function* container);
			void addPackageMethod(string name, size_t nameIndex, Function* container);

			// each method gets its own index assigned to it by the interpreter. the method's index is based on its name,
			// so we can have a method with the same name that is defined in several unrelated namespaces but that method
			// still gets the same index as the rest of the methods with the same name
			robin_map<size_t, MethodTreeEntry*> methodIndexToEntry;
			string name;
	};
}
