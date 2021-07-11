#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "symbol.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class PackageDeclaration : public Body, public ts::Package {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return PACKAGE_DECLARATION;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static PackageDeclaration* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* packageName = nullptr;
};