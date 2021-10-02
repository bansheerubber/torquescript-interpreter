#pragma once

#include <string>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

using namespace std;

class AssignStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return ASSIGN_STATEMENT;
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
		static bool ShouldParse(class AccessStatement* lvalue, Component* parent, ts::Engine* engine);
		static AssignStatement* Parse(class AccessStatement* lvalue, Component* parent, ts::Engine* engine);

		class AccessStatement* getLValue() {
			return this->lvalue;
		}

		Component* getRValue() {
			return this->rvalue;
		}
	
	private:
		static ts::instruction::InstructionType TypeToLocalOperator(TokenType type);
		static ts::instruction::InstructionType TypeToGlobalOperator(TokenType type);
		static ts::instruction::InstructionType TypeToObjectOperator(TokenType type);
		
		class AccessStatement* lvalue = nullptr;
		Component* rvalue = nullptr;
		Token assignmentToken;
};
