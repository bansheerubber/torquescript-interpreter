#pragma once

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "symbol.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class InheritanceStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return INHERITANCE_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static InheritanceStatement* Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* className = nullptr;
		Symbol* parentClass = nullptr;
};