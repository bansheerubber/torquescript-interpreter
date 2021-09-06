#pragma once

#include <vector>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"

using namespace std;

class SourceFile : public Body, public ts::Scope {
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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
};