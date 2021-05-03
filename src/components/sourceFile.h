#pragma once

#include <vector>

#include "component.h"
#include "body.h"

using namespace std;

class SourceFile : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return SOURCE_FILE;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		ts::InstructionReturn compile();

		string print();
};