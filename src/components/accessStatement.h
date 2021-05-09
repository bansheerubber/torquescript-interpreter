#pragma once

#include <string>
#include <vector>

#include "component.h"
#include "arrayStatement.h"
#include "callStatement.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"
#include "../interpreter/stack.h"

using namespace std;

struct AccessElement {
	Token token; // a token if we have one
	bool isArray;
	bool isCall;
	Component* component; // array access if there is any
};

struct AccessStatementCompiled {
	ts::InstructionReturn output;
	ts::Instruction* lastAccess;
};

// a local/global access
// examples:
// %ex, %ex.hey, %ex.hey[0], %ex[0]
// $ex, $ex.hey, $ex.hey[0], $ex[0], $ex::ex2, $ex::ex2.hey, $ex::ex2.hey[0], $ex::ex2[0]
class AccessStatement : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return ACCESS_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		ts::InstructionReturn compile();
		AccessStatementCompiled compileAccess();

		string print();
		static bool ShouldParse(Tokenizer* tokenizer, Parser* parser, bool useKeyword = false);
		static AccessStatement* Parse(
			Component* firstValue,
			Component* parent,
			Tokenizer* tokenizer,
			Parser* parser,
			bool useKeyword = false
		);

		bool isLocalVariable();
		bool isGlobalVariable();
		bool hasChain();
		bool hasArray();
		bool hasCall();
		int chainSize();

		bool IsValidLvalue();

		static bool DatablockAsSymbol;

		string getVariableName();
	
	private:
		// the tokens that make this statement up
		// can be local variables, global variables, 
		vector<AccessElement> elements;
};