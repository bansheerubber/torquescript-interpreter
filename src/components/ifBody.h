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

class IfBody : public Body {
	public:
		using Body::Body;
		~IfBody() {}
		
		ComponentType getType() {
			return IF_STATEMENT;
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
			if(child == this->conditional) {
				return true;
			}
			return false;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static IfBody* Parse(Body* body, ts::Engine* engine);

		Body* next = nullptr;
	
	private:
		Component* conditional = nullptr;
};
