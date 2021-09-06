#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

class InlineConditional : public Component {
	public:
		using Component::Component;
		~InlineConditional() {}
		
		ComponentType getType() {
			return INLINE_CONDITIONAL;
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
		static InlineConditional* Parse(Component* leftHandSide, Component* parent, ts::Engine* engine);
	
	private:
		Component* leftHandSide = nullptr;
		Component* ifTrue = nullptr;
		Component* ifFalse = nullptr;
};
