#pragma once

#include <vector>

#include "variableContext.h"
#include "objectReference.h"

using namespace std;
using namespace ts;

namespace ts {
	class Object {
		public:
			Object(class ts::Interpreter* interpreter);
			~Object();

			VariableContext properties;
			unsigned int id = 0;

			void addReference(ObjectReference* reference);
		
		private:
			vector<ObjectReference*> references;
	};
}
