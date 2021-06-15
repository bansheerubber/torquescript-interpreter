#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"
#include "accessStatement.h"
#include "callStatement.h"
#include "symbol.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

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

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static FunctionDeclaration* Parse(Body* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* name1 = nullptr; // left hand side of namespace
		Symbol* name2 = nullptr; // right hand side of namespace
		CallStatement* args = nullptr;
};
