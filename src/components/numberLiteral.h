#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

class NumberLiteral : public Component {
	public:
		using Component::Component;
		~NumberLiteral() {}
		
		ComponentType getType() {
			return NUMBER_LITERAL;
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
		static NumberLiteral* Parse(Component* parent, ts::Engine* engine);

		double getNumber();
	
	private:
		string number;
};
