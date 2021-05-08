#include "mathExpression.h"

map<TokenType, int> MathExpression::Precedence = MathExpression::CreatePrecedenceMap();

bool MathExpression::IsOperator(TokenType type) {
	return type == PLUS
		|| type == MINUS
		|| type == SLASH
		|| type == ASTERISK
		|| type == MODULUS
		|| type == NOT_EQUAL
		|| type == EQUAL
		|| type == STRING_EQUAL
		|| type == STRING_NOT_EQUAL
		|| type == LESS_THAN_EQUAL
		|| type == GREATER_THAN_EQUAL
		|| type == LESS_THAN
		|| type == GREATER_THAN
		|| type == LOGICAL_AND
		|| type == LOGICAL_OR
		|| type == SHIFT_LEFT
		|| type == SHIFT_RIGHT
		|| type == BITWISE_AND
		|| type == BITWISE_OR
		|| type == BITWISE_XOR
		|| type == APPEND
		|| type == SPC
		|| type == TAB
		|| type == NL;
}

bool MathExpression::ShouldParse(Component* lvalue, Tokenizer* tokenizer, Parser* parser) {
	if(lvalue == nullptr) {
		return (
			(
				(
					NumberLiteral::ShouldParse(tokenizer, parser)
					|| StringLiteral::ShouldParse(tokenizer, parser)
					|| BooleanLiteral::ShouldParse(tokenizer, parser)
				)
				&& MathExpression::IsOperator(tokenizer->peekToken(1).type)
			)
			|| tokenizer->peekToken().type == LEFT_PARENTHESIS
			|| tokenizer->peekToken().type == LOGICAL_NOT
			|| tokenizer->peekToken().type == BITWISE_NOT
			|| tokenizer->peekToken().type == MINUS
		);
	}
	else {
		return MathExpression::IsOperator(tokenizer->peekToken().type);
	}
}

MathExpression* MathExpression::Parse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	MathExpression* output = new MathExpression(parser);
	output->parent = parent;
	
	if(lvalue != nullptr) {
		output->elements.push_back({
			component: lvalue,
		});
		lvalue->setParent(output);
	}

	// if we start with a left parenthesis, make note of it
	bool parenthesis = false;
	if(tokenizer->peekToken().type == LEFT_PARENTHESIS) {
		tokenizer->getToken(); // absorb parenthesis
		output->elements.push_back({
			component: nullptr,
			specialOp: LEFT_PARENTHESIS_OPERATOR,
		});
		parenthesis = true;
	}

	bool expectingOperator = lvalue != nullptr;
	while(!tokenizer->eof()) {
		if(expectingOperator) {
			Token token = tokenizer->getToken();
			if(MathExpression::IsOperator(token.type)) {
				output->elements.push_back({
					component: nullptr,
					op: token,
				});
				expectingOperator = false;
			}
			else if(parenthesis && token.type == RIGHT_PARENTHESIS) {
				output->elements.push_back({
					component: nullptr,
					specialOp: RIGHT_PARENTHESIS_OPERATOR,
				});
				break; // quit out of loop since our statement is now over
			}
			else { // if we see an end to the statement, quit out
				tokenizer->unGetToken();
				break;
			}
		}
		else {
			if(tokenizer->peekToken().type == LEFT_PARENTHESIS) { // nested math expression
				output->elements.push_back({
					component: Component::AfterParse(
						MathExpression::Parse(nullptr, output, tokenizer, parser),
						output,
						tokenizer,
						parser
					),
				});
				expectingOperator = true;
			}
			else if(tokenizer->peekToken().type == LOGICAL_NOT) {
				tokenizer->getToken(); // absorb token
				output->elements.push_back({
					component: nullptr,
					specialOp: LOGICAL_NOT_OPERATOR,
				});
				expectingOperator = false;
			}
			else if(tokenizer->peekToken().type == BITWISE_NOT) {
				tokenizer->getToken(); // absorb token
				output->elements.push_back({
					component: nullptr,
					specialOp: BITWISE_NOT_OPERATOR,
				});
				expectingOperator = false;
			}
			else if(tokenizer->peekToken().type == MINUS) {
				tokenizer->getToken(); // absorb token
				output->elements.push_back({
					component: nullptr,
					specialOp: MINUS_OPERATOR,
				});
				expectingOperator = false;
			}
			// handles literals, accesses, calls, etc
			else if(Component::ShouldParse(output, tokenizer, parser)) {
				output->elements.push_back({
					component: Component::Parse(output, tokenizer, parser),
				});
				expectingOperator = true;
			}
			else {
				break; // quit out since we didn't recognize something
			}
		}
	}

	// under this circumstance, we need to check if there is more math expression to parse
	if(parenthesis && parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(output, tokenizer, parser)) {
		return MathExpression::Parse(output, parent, tokenizer, parser);
	}
	else if(parenthesis && output->elements.back().specialOp != RIGHT_PARENTHESIS_OPERATOR) {
		parser->error("unclosed parenthesis");
	}

	if(expectingOperator == false) {
		parser->error("was expecting evaluatable expression, number literal, boolean literal, or string literal");
	}

	return output;
}

string MathExpression::print() {
	string output;
	MathElement lastElement;
	for(MathElement element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->print();
		}
		else if(element.specialOp == LEFT_PARENTHESIS_OPERATOR) {
			output += "(";
		}
		else if(element.specialOp == RIGHT_PARENTHESIS_OPERATOR) {
			output += ")";
		}
		else if(element.specialOp == LOGICAL_NOT_OPERATOR) {
			output += "!";
		}
		else if(element.specialOp == BITWISE_NOT_OPERATOR) {
			output += "~";
		}
		else if(element.specialOp == MINUS_OPERATOR) {
			if(this-parser->minified && lastElement.op.type == MINUS) {
				output += " -";
			}
			else {
				output += "-";
			}
		}
		else {
			if(this->parser->minified) {
				if(element.op.type == SPC || element.op.type == TAB || element.op.type == NL || element.op.type == MODULUS) {
					output += " " + element.op.lexeme + " ";
					continue;
				}
			}
			output += this->parser->space + element.op.lexeme + this->parser->space;
		}
		lastElement = element;
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}

	return output;
}

map<TokenType, int> MathExpression::CreatePrecedenceMap() {
	map<TokenType, int> output;
	output[PLUS] = 1;
	output[MINUS] = 1;
	output[ASTERISK] = 2;
	output[SLASH] = 2;
	return output;
}

// helper function for parsing Precedence
void MathExpression::createInstructions(
	vector<Operation> &operands,
	vector<Operation> &operators,
	relative_stack_location &stackPointer
) {
	// keep track of the current precedence, since we will be going down the operators list until we hit something
	// that isn't equal to this precedence
	int topPrecedence = MathExpression::Precedence[operators.back().element.op.type];
	
	// pointers into the operands/operators vector for when we need to evaluate expressions
	unsigned int operandIndex = operands.size() - 1, operatorIndex = 0;
	for(int i = operators.size() - 1; i >= 0; i--) { // find our place in the vectors
		if(MathExpression::Precedence[operators[i].element.op.type] != topPrecedence) {
			operatorIndex = i + 1; // find the first operator we will start performing mathematics on
			break;
		}
		operandIndex--; // go further down the operand list
	}

	// keep going until we have evaluated all the expressions starting frm inside of the vector
	// until the end of the vector, removing math elements as we go
	while(operatorIndex < operators.size()) {
		Operation op = operators[operatorIndex];
		operators.erase(operators.begin() + operatorIndex); // remove the operator from the vector

		// get the operands
		Operation lvalue = operands[operandIndex];
		Operation rvalue = operands[operandIndex + 1];

		if(lvalue.element.component != nullptr) {
			printf("%s", lvalue.element.component->print().c_str());
		}
		else {
			printf("stack{%d}", lvalue.stack);
		}

		printf(" %s ", this->parser->tokenizer->typeToName(op.element.op.type));

		if(rvalue.element.component != nullptr) {
			printf("%s", rvalue.element.component->print().c_str());
		}
		else {
			printf("stack{%d}", rvalue.stack);
		}

		printf(", saved to stack{%d}\n", stackPointer);

		operands[operandIndex] = {
			stack: stackPointer++,
		}; // add result of expression back to the operand stack
		operands.erase(operands.begin() + operandIndex + 1); // remove the operand from the vector
	}
}

void MathExpression::parsePrecedence() {
	vector<Operation> operands, operators;
	relative_stack_location stackPointer = 0;

	for(MathElement element: this->elements) {
		if(element.component != nullptr) { // if the element is an operand, push it to the operand stack
			operands.push_back({
				element: element,
			});
		}
		else {
			while(
				operators.size() > 0
				&& MathExpression::Precedence[operators.back().element.op.type] > MathExpression::Precedence[element.op.type]
			) {
				this->createInstructions(operands, operators, stackPointer);
			}

			operators.push_back({
				element: element,
			});
		}
	}

	while(operators.size() > 0) {
		this->createInstructions(operands, operators, stackPointer);
	}
}

ts::InstructionReturn MathExpression::compile() {
	this->parsePrecedence();
	return {};
}