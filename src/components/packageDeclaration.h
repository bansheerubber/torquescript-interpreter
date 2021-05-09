#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "component.h"
#include "body.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"
#include "symbol.h"

using namespace std;

class PackageDeclaration : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return PACKAGE_DECLARATION;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static PackageDeclaration* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
	
	private:
		Symbol* packageName = nullptr;
};