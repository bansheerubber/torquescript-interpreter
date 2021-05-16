#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "component.h"
#include "body.h"
#include "caseBody.h"
#include "defaultBody.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

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

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static SwitchBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
	
	private:
		Token switchType;
		Component* conditional = nullptr;
};