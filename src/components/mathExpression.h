#pragma once

#include <deque>
#include <queue>
#include <stack>
#include <string>

#include "component.h"
#include "accessStatement.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "booleanLiteral.h"
#include "../parser/parser.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"
#include "../interpreter/stack.h"
#include "../interpreter/entry.h"

// forward declare interpreter
namespace ts {
	class Interpreter;
}

enum SpecialOperator {
	INVALID_OPERATOR,
	LEFT_PARENTHESIS_OPERATOR,
	RIGHT_PARENTHESIS_OPERATOR,
	LOGICAL_NOT_OPERATOR,
	BITWISE_NOT_OPERATOR,
	MINUS_OPERATOR,
};

struct MathElement {
	Component* component;
	Token op;
	SpecialOperator specialOp;
};

struct LogicalElement {
	vector<MathElement*>* list;
	Token op;
};

struct PostfixElement {
	MathElement* element;
	deque<SpecialOperator> unary;
};

struct Operation { // used for math evaluation algorithm
	MathElement element;
	relative_stack_location stack;
};

class MathExpression : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return MATH_EXPRESSION;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		bool shouldPushToStack(Component* child) {
			return true;
		}

		ts::InstructionReturn compile(ts::Interpreter* interpreter);

		string print();
		string printJSON();
		static bool ShouldParse(Component* lvalue, Tokenizer* tokenizer, Parser* parser);
		static MathExpression* Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static bool IsOperator(TokenType type);
	
	private:
		vector<MathElement> elements;

		static map<TokenType, int> CreatePrecedenceMap();
		static ts::instruction::InstructionType TypeToOperator(TokenType type);
		static map<TokenType, int> Precedence;

		ts::InstructionReturn compileList(vector<MathElement*>* list, ts::Interpreter* interpreter);
		vector<PostfixElement> convertToPostfix(vector<MathElement*>* list, bool prefixMod = false);
};