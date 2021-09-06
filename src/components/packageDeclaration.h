#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "symbol.h"

using namespace std;

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

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static PackageDeclaration* Parse(Component* parent, ts::Engine* engine);
	
	private:
		Symbol* packageName = nullptr;
};
