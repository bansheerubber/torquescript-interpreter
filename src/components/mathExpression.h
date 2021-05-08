#pragma once

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

		ts::InstructionReturn compile();

		string print();
		static bool ShouldParse(Component* lvalue, Tokenizer* tokenizer, Parser* parser);
		static MathExpression* Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static bool IsOperator(TokenType type);
	
	private:
		vector<MathElement> elements;

		static map<TokenType, int> CreatePrecedenceMap();
		static ts::instruction::MathematicsOperator TypeToOperator(TokenType type);
		static map<TokenType, int> Precedence;

		ts::InstructionReturn createInstructions(
			vector<Operation> &operands,
			vector<Operation> &operators,
			relative_stack_location &stackPointer
		);
};