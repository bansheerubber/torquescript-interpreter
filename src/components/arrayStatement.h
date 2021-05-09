#pragma once

#include <vector>

#include "component.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

struct ArrayElement {
	Component* component;
	bool isComma;
};

// parses the [...] part of an access
class ArrayStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return ARRAY_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser);
		static ArrayStatement* Parse(Component* parent, Tokenizer* tokenizer, Parser* parser);

		int getDimensions();
	
	private:
		// can be literals, a mathematical statement, access statements, etc
		vector<ArrayElement> elements;
};