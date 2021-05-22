#pragma once

#include <vector>

#include "component.h"
#include "body.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class SourceFile : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return SOURCE_FILE;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
};