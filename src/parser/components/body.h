#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "component.h"

using namespace std;

class Body : public Component {
	public:
		ComponentType getType() {
			return INVALID_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print() {
			return "";
		}

		string printBody();

		void addChild(Component* component);
		Component* getLastChild();
	
	protected:
		vector<Component*> children;
};