#pragma once

#include <vector>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class DefaultBody : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return DEFAULT_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static DefaultBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
};