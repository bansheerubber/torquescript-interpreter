#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class NewStatement : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return NEW_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->className || child == this->arguments) {
				return false;
			}
			return true;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static NewStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Component* className = nullptr;
		Component* arguments = nullptr;
};