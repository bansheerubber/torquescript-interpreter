#pragma once

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "callStatement.h"
#include "symbol.h"

class NamespaceStatement : public Component {
	public:
		using Component::Component;
		~NamespaceStatement() {}
		
		ComponentType getType() {
			return PARENT_STATEMENT;
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
		static NamespaceStatement* Parse(Component* parent, ts::Engine* engine);
	
	private:
		Symbol* name = nullptr;
		Symbol* operation = nullptr;
		CallStatement* call = nullptr;
};
