#pragma once

#include <string>
#include <vector>

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

#include "inheritanceStatement.h"
#include "symbol.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

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

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static DatablockDeclaration* Parse(Body* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* typeName = nullptr; // type name of datablock
		InheritanceStatement* className = nullptr; // class name or inheritence expression
};
