#pragma once

#include <string>

#include "component.h"
#include "body.h"
#include "../compiler/loops.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

// forward declare interpreter
namespace ts {
	class Interpreter;
}

class ForBody : public Body, public ts::LoopsContext {
	public:
		using Body::Body;
		
		ComponentType getType() {
			return FOR_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			if(child == this->initialization || child == this->conditional || child == this->increment) {
				return false;
			}
			return true;
		}

		bool shouldPushToStack(Component* child) {
			if(child == this->conditional) {
				return true;
			}
			return false;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(Tokenizer* tokenizer, class Parser* parser);
		static ForBody* Parse(Body* body, Tokenizer* tokenizer, class Parser* parser);
	
	private:
		Component* initialization = nullptr;
		Component* conditional = nullptr;
		Component* increment = nullptr;
};