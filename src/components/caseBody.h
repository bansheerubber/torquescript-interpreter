#pragma once

#include <vector>

#include "component.h"
#include "body.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

struct CaseElement {
	Component* component;
	bool isOr;
};

class CaseBody : public Body {
	public:
		using Body::Body;
		~CaseBody() {}
		
		ComponentType getType() {
			return CASE_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			for(CaseElement element: this->conditionals) {
				if(element.component == child) {
					return false;
				}
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
		static CaseBody* Parse(Body* body, ts::Engine* engine);
	
	private:
		vector<CaseElement> conditionals;
		friend class SwitchBody;
};
