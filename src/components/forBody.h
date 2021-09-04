#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../compiler/loops.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

class ForBody : public Body, public ts::LoopsContext {
	public:
		using Body::Body;
		~ForBody() {}
		
		ComponentType getType() {
			return FOR_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->initialization || child == this->conditional || child == this->increment) {
				return false;
			}
			return true;
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
		static ForBody* Parse(Body* body, ts::Engine* engine);
	
	private:
		Component* initialization = nullptr;
		Component* conditional = nullptr;
		Component* increment = nullptr;
};
