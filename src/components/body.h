#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "component.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class Body : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return INVALID_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		virtual ts::InstructionReturn compile(ts::Interpreter* interpreter) = 0;

		string print() {
			return "";
		}

		string printBody();

		void addChild(Component* component);
		Component* getLastChild();
	
	protected:
		vector<Component*> children;
};