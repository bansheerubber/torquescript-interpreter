#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Engine;

	namespace sl {
		class SimObject : public Object {

		};

		Entry* SimObject__test(Engine* engine, size_t argc, Entry* args);
		Entry* SimObject__getId(Engine* engine, size_t argc, Entry* args);
		Entry* SimObject__delete(Engine* engine, size_t argc, Entry* args);
	}
}