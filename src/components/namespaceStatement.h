#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "callStatement.h"
#include "symbol.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class NamespaceStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return PARENT_STATEMENT;
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
		static NamespaceStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* name = nullptr;
		Symbol* operation = nullptr;
		CallStatement* call = nullptr;
};
