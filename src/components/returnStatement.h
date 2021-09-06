#pragma once

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

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
		
		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static ReturnStatement* Parse(Component* parent, ts::Engine* engine);
	
	private:
		Component* operation = nullptr;
};
