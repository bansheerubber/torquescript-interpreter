#pragma once

#include <string>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

class StringLiteral : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return STRING_LITERAL;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static StringLiteral* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Token value;
};