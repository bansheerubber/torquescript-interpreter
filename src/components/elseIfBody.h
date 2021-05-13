#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
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

		ts::InstructionReturn compile();
		ElseIfBodyCompiled compileElseIf();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ElseIfBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);

		Body* next = nullptr;
	
	private:
		Component* conditional = nullptr;
};