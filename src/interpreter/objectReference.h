#pragma once

namespace ts {
	class ObjectReference {
		public:
			ObjectReference(class Object* object);
			~ObjectReference();
			class Object* object = nullptr;
		
		private:
			friend class Object;

			ObjectReference* next = nullptr;
			ObjectReference* previous = nullptr;
	};
}
