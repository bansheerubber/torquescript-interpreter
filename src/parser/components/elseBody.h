#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "component.h"
#include "body.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

using namespace std;

class ElseBody : public Body {
	public:
		using Body::Body;

		ComponentType getType() {
			return ELSE_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ElseBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
};