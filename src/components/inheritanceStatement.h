#pragma once

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "symbol.h"

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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Component* lvalue, Component* parent, ts::Engine* engine);
		static InheritanceStatement* Parse(Component* lvalue, Component* parent, ts::Engine* engine);

		friend class NewStatement;
	
	private:
		Component* className = nullptr;
		Symbol* parentClass = nullptr;
};
