#pragma once

#include <string>

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

class NumberLiteral : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return NUMBER_LITERAL;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static NumberLiteral* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);

		double getNumber();
	
	private:
		string number;
};