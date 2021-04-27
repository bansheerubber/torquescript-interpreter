#pragma once

#include <string>

#include "component.h"
#include "accessStatement.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "booleanLiteral.h"
#include "../parser.h"
#include "../../tokenizer/token.h"
#include "../../tokenizer/tokenizer.h"

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

class MathExpression : public Component {
	public:
		using Component::Component;
		
		ComponentType getType() {
			return MATH_EXPRESSION;
		}

		bool requiresSemicolon(Component* child) {
			return false;
		}

		string print();
		static bool ShouldParse(Component* lvalue, Tokenizer* tokenizer, Parser* parser);
		static MathExpression* Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser);
		static bool IsOperator(TokenType type);
	
	private:
		vector<MathElement> elements;
};