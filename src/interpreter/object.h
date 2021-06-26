#pragma once

#include <vector>

#include "objectReference.h"
#include "variableContext.h"

using namespace std;
using namespace ts;

namespace ts {
	class Object {
		public:
			Object(class ts::Interpreter* interpreter, string nameSpace, int namespaceIndex);
			~Object();

			VariableContext properties;
			size_t id = 0;

			void addReference(ObjectReference* reference);

			string nameSpace;
			int namespaceIndex;
		
		private:
			vector<ObjectReference*> references;
	};
}
