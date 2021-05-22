#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class ReturnStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return RETURN_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return true;
		}
		
		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static ReturnStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* operation = nullptr;
};
