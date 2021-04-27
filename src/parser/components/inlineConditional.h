#pragma once

#include <string>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

class InlineConditional : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return INLINE_CONDITIONAL;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static InlineConditional* Parse(Component* leftHandSide, Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* leftHandSide = nullptr;
		Component* ifTrue = nullptr;
		Component* ifFalse = nullptr;
};