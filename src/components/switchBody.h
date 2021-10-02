#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

class SwitchBody : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return SWITCH_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == conditional) {
				return false;
			}
			else {
				return true;
			}
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static SwitchBody* Parse(Body* body, ts::Engine* engine);
	
	private:
		Token switchType;
		Component* conditional = nullptr;
};
