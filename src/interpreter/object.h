#pragma once

#include <vector>

#include "objectReference.h"
#include "variableContext.h"

using namespace std;
using namespace ts;

namespace ts {
	class Object {
		public:
			Object(class ts::Interpreter* interpreter, string nameSpace, string inheritedName, size_t namespaceIndex);
			~Object();

			VariableContext properties;
			size_t id = 0;

			void addReference(ObjectReference* reference);
			void removeReference(ObjectReference* reference);

			void setName(string &name);

			string nameSpace;
			size_t namespaceIndex;
		
		private:
			void inherit(Object* parent);
			ObjectReference* list = nullptr;
			ObjectReference* top = nullptr;
			string name;
	};
}
