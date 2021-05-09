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

class ForBody : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return FOR_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->initialization || child == this->conditional || child == this->increment) {
				return false;
			}
			return true;
		}

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ForBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
	
	private:
		Component* initialization = nullptr;
		Component* conditional = nullptr;
		Component* increment = nullptr;
};