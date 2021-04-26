#pragma once

#include <string>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

using namespace std;

class Symbol : public Component {
	public:
		ComponentType getType() {
			return SYMBOL_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static Symbol* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		string value;
};