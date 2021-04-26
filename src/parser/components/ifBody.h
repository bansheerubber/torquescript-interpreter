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

class IfBody : public Body {
	public:
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

		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static IfBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);

		string print();

		Body* next;
	
	private:
		Component* conditional;
};