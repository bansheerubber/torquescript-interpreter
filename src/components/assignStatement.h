#pragma once

#include <string>

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

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

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::Scope* scope);

		string print();
		string printJSON();
		static bool ShouldParse(class AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static AssignStatement* Parse(class AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);

		class AccessStatement* getLValue() {
			return this->lvalue;
		}

		Component* getRValue() {
			return this->rvalue;
		}
	
	private:
		static ts::instruction::InstructionType TypeToLocalOperator(TokenType type);
		static ts::instruction::InstructionType TypeToObjectOperator(TokenType type);
		
		class AccessStatement* lvalue = nullptr;
		Component* rvalue = nullptr;
		Token assignmentToken;
};