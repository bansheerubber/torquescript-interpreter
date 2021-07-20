#pragma once

#include "dynamicArray.h"
#include "function.h"

#define PACKAGED_FUNCTION_LIST_START_SIZE 5

namespace ts {
	// describes the package hierarchy for a particular function
	class PackagedFunctionList {
		public:
			PackagedFunctionList();
			PackagedFunctionList(string functionName, string functionNamespace = string());

			// if the initial function is a part of the standard library
			bool isTSSL = false;

			string functionName;
			string functionNamespace;

			void defineInitialFunction(Function* function);
			void addPackageFunction(Function* function);
			void deactivateFunction(Function* function);
			void activateFunction(Function* function);

			int getNextValidIndex(int currentIndex);

			Function* operator[](size_t index) {
				return this->functions[index];
			}

			size_t topValidIndex = 0;
		
		private:
			DynamicArray<Function*, PackagedFunctionList> functions;
			void findValidFunction();
	};
}
