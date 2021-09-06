#pragma once

#include <string>
#include <vector>

#include "component.h"
#include "../engine/engine.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/token.h"

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
	friend class NewStatement;
	
	public:
		using Component::Component;
		~AccessStatement() {}
		
		ComponentType getType() {
			return ACCESS_STATEMENT;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return true;
		}

		ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context);
		AccessStatementCompiled compileAccess(ts::Engine* engine, ts::CompilationContext context);

		string print();
		string printJSON();
		static bool ShouldParse(ts::Engine* engine, bool useKeyword = false);
		static AccessStatement* Parse(
			Component* firstValue,
			Component* parent,
			ts::Engine* engine,
			bool useKeyword = false
		);

		bool isLocalVariable();
		bool isGlobalVariable();
		bool startsWithFunction();
		bool hasChain();
		bool hasArray();
		bool hasCall();
		size_t chainSize();
		size_t getStackIndex(ts::Scope* scope);

		bool isValidLValue();

		static bool DatablockAsSymbol;

		string getVariableName();
	
	private:
		// the tokens that make this statement up
		// can be local variables, global variables, 
		vector<AccessElement> elements;
};
