#pragma once

#include <vector>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

struct CallElement {
	Component* component;
	bool isComma;
};

// parses the [...] part of an access
class CallStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return CALL_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return true;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine);
		static CallStatement* Parse(Component* parent, ts::Engine* engine);
		
		pair<
			vector<CallElement>::iterator,
			vector<CallElement>::iterator
		> getElements();

		CallElement &getElement(size_t index);

		size_t getElementCount();
	
	private:
		// can be literals, a mathematical statement, access statements, etc
		vector<CallElement> elements;
};
