#pragma once

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/tokenizer.h"
#include "../../tokenizer/token.h"

class PostfixStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return POSTFIX_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static PostfixStatement* Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Token op;
		Component* lvalue;
};
