#pragma once

#include <string>

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class BooleanLiteral : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return BOOLEAN_LITERAL;
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
		static BooleanLiteral* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);

		bool getBoolean();
	
	private:
		Token value;
};