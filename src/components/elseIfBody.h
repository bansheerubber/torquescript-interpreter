#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

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

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::Scope* scope);
		ElseIfBodyCompiled compileElseIf(ts::Interpreter* interpreter, ts::Scope* scope);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ElseIfBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);

		Body* next = nullptr;
	
	private:
		Component* conditional = nullptr;
};