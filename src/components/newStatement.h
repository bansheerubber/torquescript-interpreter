#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"
#include "accessStatement.h"
#include "assignStatement.h"
#include "callStatement.h"
#include "inheritanceStatement.h"
#include "mathExpression.h"
#include "symbol.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class NewStatement : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return NEW_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->className || child == this->arguments) {
				return false;
			}
			return true;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static NewStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* className = nullptr;
		Component* arguments = nullptr;
};