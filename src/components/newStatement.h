#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "callStatement.h"

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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static NewStatement* Parse(Component* parent, ts::Engine* engine);
	
	private:
		Component* className = nullptr;
		CallStatement* arguments = nullptr;

		Component* classProperty = nullptr;
		Component* superClassProperty = nullptr;
};
