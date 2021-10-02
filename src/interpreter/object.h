#pragma once

#include <vector>

#include "objectReference.h"
#include "variableContext.h"

using namespace std;
using namespace ts;

namespace ts {
	class Object {
		public:
			Object(class ts::Interpreter* interpreter, string nameSpace, string inheritedName, class MethodTree* methodTree, class MethodTree* typeMethodTree);
			~Object();

			VariableContext properties;
			size_t id = 0;

			void addReference(ObjectReference* reference);
			void removeReference(ObjectReference* reference);

			void setName(string &name);

			string nameSpace;
			class MethodTree* methodTree;
			class MethodTree* typeMethodTree;
		
		private:
			void inherit(Object* parent);
			ObjectReference* list = nullptr;
			ObjectReference* top = nullptr;
			string name;
			ObjectWrapper* wrapper;
	};

	struct ObjectWrapper {
		Object* object;
		void* data; // programmer-defined data for lib

		friend class Object;

		ObjectWrapper() {}
		ObjectWrapper(Object* object, void* data = nullptr) {
			this->object = object;
			this->data = data;
		}

		~ObjectWrapper() {
			delete this->object;
			delete this->data;
		}
	};

	#define TS_OBJECT_CONSTRUCTOR(name)		void (*name)(ObjectWrapper* wrapper)
	ObjectWrapper* CreateObject(
		class ts::Interpreter* interpreter,
		string nameSpace,
		string inheritedName,
		class MethodTree* methodTree,
		class MethodTree* typeMethodTree,
		void* data = nullptr
	);
}
