#pragma once

#include <string>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

class Comment : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return COMMENT_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static Comment* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		string comment;
};