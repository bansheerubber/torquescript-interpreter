#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

class BreakStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return BREAK_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}
		
		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static BreakStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
};
