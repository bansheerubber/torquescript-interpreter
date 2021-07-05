#pragma once

#include <deque>
#include <stack>
#include <string>

#include "component.h"
#include "../parser/parser.h"
#include "../compiler/scope.h"
#include "../tokenizer/token.h"
#include "../tokenizer/tokenizer.h"

#include "../interpreter/stack.h"

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

		ts::InstructionReturn compile(ts::Interpreter* interpreter, ts::CompilationContext context);

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

		ts::InstructionReturn compileList(vector<MathElement*>* list, ts::Interpreter* interpreter, ts::CompilationContext context);
		vector<PostfixElement> convertToPostfix(vector<MathElement*>* list, bool prefixMod = false);
};