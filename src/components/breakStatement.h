#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class BreakStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return BREAK_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}
		
		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::Scope* scope);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static BreakStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
};
