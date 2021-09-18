#pragma once

#include "../interpreter/entry.h"

namespace ts {
	class Engine;

	namespace sl {
		Entry* schedule(Engine* engine, unsigned int argc, Entry* args);

		Entry* SimObject__schedule(Engine* engine, unsigned int argc, Entry* args);
	}
}
