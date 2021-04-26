#pragma once

#include <vector>

#include "component.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

using namespace std;

struct CallElement {
	Component* component;
	bool isComma;
};

// parses the [...] part of an access
class CallStatement : public Component {
	public:
		ComponentType getType() {
			return CALL_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static CallStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);
		
		pair<
			vector<CallElement>::iterator,
			vector<CallElement>::iterator
		> getElements();
	
	private:
		// can be literals, a mathematical statement, access statements, etc
		vector<CallElement> elements;
};