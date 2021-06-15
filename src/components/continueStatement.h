#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class ContinueStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return CONTINUE_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static ContinueStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
};
