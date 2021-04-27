#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/tokenizer.h"
#include "../../tokenizer/token.h"
#include "accessStatement.h"
#include "callStatement.h"
#include "symbol.h"

using namespace std;

class FunctionDeclaration : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return FUNCTION_DECLARATION;
		}

		bool requiresSemicolon(Component* child) {
			if(this->name1 == child || this->name2 == child || this->args == child) {
				return false;
			}
			return true;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static FunctionDeclaration* Parse(Body* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* name1; // left hand side of namespace
		Symbol* name2; // right hand side of namespace
		CallStatement* args;
};
