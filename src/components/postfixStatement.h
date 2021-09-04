#pragma once

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "accessStatement.h"

class PostfixStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return POSTFIX_STATEMENT;
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
		static PostfixStatement* Parse(AccessStatement* lvalue, Component* parent, ts::Engine* engine);
	
	private:
		Token op;
		AccessStatement* lvalue = nullptr;
};
