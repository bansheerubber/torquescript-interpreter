#pragma once

namespace ts {
	class ObjectReference {
		public:
			ObjectReference(class Object* object);
			class Object* object = nullptr;
	};
}
