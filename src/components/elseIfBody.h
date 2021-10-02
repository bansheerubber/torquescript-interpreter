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

struct ElseIfBodyCompiled {
	ts::InstructionReturn output;
	ts::Instruction* lastJump;
	ts::Instruction* conditionalJump;
};

class ElseIfBody : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return ELSE_IF_STATEMENT;
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
		ElseIfBodyCompiled compileElseIf(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static ElseIfBody* Parse(Body* body, ts::Engine* engine);

		Body* next = nullptr;
	
	private:
		Component* conditional = nullptr;
};
