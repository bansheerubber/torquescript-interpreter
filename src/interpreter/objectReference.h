#pragma once

#include <cstddef>

using namespace std;

namespace ts {
	class ObjectReference {
		public:
			ObjectReference(class Object* object);
			~ObjectReference();
			class Object* object = nullptr;

			size_t id = 0;
		
		private:
			friend class Object;

			ObjectReference* next = nullptr;
			ObjectReference* previous = nullptr;
	};
}
