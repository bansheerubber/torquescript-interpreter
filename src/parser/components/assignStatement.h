#pragma once

#include <string>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/tokenizer.h"
#include "../../tokenizer/token.h"

using namespace std;

class AssignStatement : public Component {
	public:
		ComponentType getType() {
			return ASSIGN_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

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
		class AccessStatement* lvalue;
		Component* rvalue;
		Token assignmentToken;
};