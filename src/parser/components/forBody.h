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

class ForBody : public Body {
	public:
		ComponentType getType() {
			return FOR_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->initialization || child == this->conditional || child == this->increment) {
				return false;
			}
			return true;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ForBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
	
	private:
		Component* initialization;
		Component* conditional;
		Component* increment;
};