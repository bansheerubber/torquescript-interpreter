#pragma once

#include <vector>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

class DefaultBody : public Body {
	public:
		using Body::Body;
		~DefaultBody() {}
		
		ComponentType getType() {
			return DEFAULT_STATEMENT;
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
		static bool ShouldParse(ts::Engine* engine);
		static DefaultBody* Parse(Body* body, ts::Engine* engine);
};