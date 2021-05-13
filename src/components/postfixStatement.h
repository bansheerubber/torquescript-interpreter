#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"
#include "accessStatement.h"

class PostfixStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return POSTFIX_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static PostfixStatement* Parse(AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Token op;
		AccessStatement* lvalue = nullptr;
};
