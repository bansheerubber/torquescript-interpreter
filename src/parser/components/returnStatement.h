#pragma once

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/tokenizer.h"
#include "../../tokenizer/token.h"

class ReturnStatement : public Component {
	public:
		ComponentType getType() {
			return RETURN_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static ReturnStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* operation;
};
