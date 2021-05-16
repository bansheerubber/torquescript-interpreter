#pragma once

#include <string>

#include "component.h"
#include "../parser/parser.h"
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

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		static bool ShouldParse(class AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static AssignStatement* Parse(class AccessStatement* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);

		class AccessStatement* getLvalue() {
			return this->lvalue;
		}

		Component* getRValue() {
			return this->rvalue;
		}
	
	private:
		class AccessStatement* lvalue = nullptr;
		Component* rvalue = nullptr;
		Token assignmentToken;
};