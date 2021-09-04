#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

class Symbol : public Component {
	public:
		using Component::Component;
		~Symbol() {}
		
		ComponentType getType() {
			return SYMBOL_STATEMENT;
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
		static bool ShouldParseAlphabeticToken(ts::Engine* engine);
		static Symbol* Parse(Component* parent, ts::Engine* engine);
	
	private:
		string value;
};
