#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Engine;

	namespace sl {
		class ScriptObject : public Object {

		};

		Entry* ScriptObject__test(Engine* engine, size_t argc, Entry* args);
	}
}