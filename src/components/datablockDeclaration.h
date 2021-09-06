#pragma once

#include <string>
#include <vector>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "inheritanceStatement.h"
#include "symbol.h"

using namespace std;

class DatablockDeclaration : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return DATABLOCK_DECLARATION;
		}

		bool requiresSemicolon(Component* child) {
			return true;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static DatablockDeclaration* Parse(Body* parent, ts::Engine* engine);
	
	private:
		Symbol* typeName = nullptr; // type name of datablock
		class CallStatement* className = nullptr; // class name or inheritence expression
};
