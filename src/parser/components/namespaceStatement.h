#pragma once

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/tokenizer.h"
#include "../../tokenizer/token.h"
#include "callStatement.h"
#include "symbol.h"

class NamespaceStatement : public Component {
	public:
		ComponentType getType() {
			return PARENT_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static NamespaceStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* name;
		Symbol* operation;
		CallStatement* call;
};
