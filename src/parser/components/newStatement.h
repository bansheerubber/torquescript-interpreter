#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"
#include "accessStatement.h"
#include "assignStatement.h"
#include "callStatement.h"
#include "symbol.h"

class NewStatement : public Body {
	public:
		ComponentType getType() {
			return NEW_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->className || child == this->arguments) {
				return false;
			}
			return true;
		}

		string print();
		
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static NewStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* className;
		CallStatement* arguments;
};