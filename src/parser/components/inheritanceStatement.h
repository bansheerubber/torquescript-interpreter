#pragma once

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"
#include "symbol.h"

class InheritanceStatement : public Component {
	public:
		ComponentType getType() {
			return INHERITANCE_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static InheritanceStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* className;
		Symbol* parentClass;
};