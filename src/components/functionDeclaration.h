#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "callStatement.h"
#include "symbol.h"

using namespace std;

class FunctionDeclaration : public Body, public ts::Scope {
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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static FunctionDeclaration* Parse(Body* parent, ts::Engine* engine);
	
	private:
		Symbol* name1 = nullptr; // left hand side of namespace
		Symbol* name2 = nullptr; // right hand side of namespace
		CallStatement* args = nullptr;
};
