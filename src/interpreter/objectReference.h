#pragma once

#include <cstddef>

using namespace std;

namespace ts {
	class ObjectReference {
		public:
			ObjectReference(struct ObjectWrapper* object);
			ObjectReference(ObjectReference* objectReference);
			~ObjectReference();
			struct ObjectWrapper* objectWrapper = nullptr;

			unsigned long id = 0;
		
		private:
			friend class Object;

			ObjectReference* next = nullptr;
			ObjectReference* previous = nullptr;
	};
}
