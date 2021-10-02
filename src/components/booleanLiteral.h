#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static BooleanLiteral* Parse(Component* parent, ts::Engine* engine);

		bool getBoolean();
	
	private:
		Token value;
};
