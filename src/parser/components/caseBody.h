#pragma once

#include <vector>

#include "component.h"
#include "body.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

using namespace std;

struct CaseElement {
	Component* component;
	bool isOr;
};

class CaseBody : public Body {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return IF_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			for(CaseElement element: this->conditionals) {
				if(element.component == child) {
					return false;
				}
			}
			return true;
		}

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static CaseBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
	
	private:
		vector<CaseElement> conditionals;
};