#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

class StringLiteral : public Component {
	public:
		using Component::Component;
		~StringLiteral() {}
		
		ComponentType getType() {
			return STRING_LITERAL;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static StringLiteral* Parse(Component* parent, ts::Engine* engine);

		string getString();
	
	private:
		Token value;
		string valueString;
};
