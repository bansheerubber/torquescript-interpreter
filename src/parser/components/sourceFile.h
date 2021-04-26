#pragma once

#include <vector>

#include "component.h"
#include "body.h"

using namespace std;

class SourceFile : public Body {
	public:
		ComponentType getType() {
			return SOURCE_FILE;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		string print();
};