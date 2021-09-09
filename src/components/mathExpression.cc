#include "mathExpression.h"
#include "../interpreter/interpreter.h"

#include "accessStatement.h"
#include "booleanLiteral.h"
#include "../interpreter/entry.h"
#include "inlineConditional.h"
#include "numberLiteral.h"
#include "stringLiteral.h"
#include "symbol.h"

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

bool MathExpression::ShouldParse(Component* lvalue, ts::Engine* engine) {
	Token &token = engine->tokenizer->peekToken();
	if(lvalue == nullptr) {
		return (
			(
				(
					NumberLiteral::ShouldParse(engine)
					|| StringLiteral::ShouldParse(engine)
					|| BooleanLiteral::ShouldParse(engine)
					|| Symbol::ShouldParse(engine)
				)
				&& MathExpression::IsOperator(engine->tokenizer->peekToken(1).type)
			)
			|| token.type == LEFT_PARENTHESIS
			|| token.type == LOGICAL_NOT
			|| token.type == BITWISE_NOT
			|| token.type == MINUS
		);
	}
	else {
		return MathExpression::IsOperator(token.type);
	}
}

MathExpression* MathExpression::Parse(Component* lvalue, Component* parent, ts::Engine* engine) {
	MathExpression* output = new MathExpression(engine);
	output->parent = parent;

	if(lvalue != nullptr) {
		output->elements.push_back((MathElement){
			component: lvalue,
		});
		lvalue->setParent(output);
	}

	// if we start with a left parenthesis, make note of it
	bool parenthesis = false;
	if(engine->tokenizer->peekToken().type == LEFT_PARENTHESIS) {
		engine->tokenizer->getToken(); // absorb parenthesis
		output->elements.push_back((MathElement){
			component: nullptr,
			specialOp: LEFT_PARENTHESIS_OPERATOR,
		});
		parenthesis = true;
	}

	bool expectingOperator = lvalue != nullptr;
	while(!engine->tokenizer->eof()) {
		if(expectingOperator) {
			Token token = engine->tokenizer->peekToken();
			if(MathExpression::IsOperator(token.type)) {
				output->elements.push_back((MathElement){
					component: nullptr,
					op: token,
				});
				expectingOperator = false;
				engine->tokenizer->getToken();
			}
			else if(InlineConditional::ShouldParse(engine) && output->elements[0].specialOp == LEFT_PARENTHESIS_OPERATOR) {
				output->elements.erase(output->elements.begin());
				
				MathExpression* newParent = new MathExpression(engine);
				newParent->elements.push_back((MathElement){
					component: nullptr,
					specialOp: LEFT_PARENTHESIS_OPERATOR,
				});

				newParent->parent = parent;
				output->parent = newParent;
				newParent->elements.push_back((MathElement){
					component: InlineConditional::Parse(output, newParent, engine)
				});

				engine->parser->expectToken(RIGHT_PARENTHESIS);
				newParent->elements.push_back((MathElement){
					component: nullptr,
					specialOp: RIGHT_PARENTHESIS_OPERATOR,
				});

				output = newParent;
				break;
			}
			else if(parenthesis && token.type == RIGHT_PARENTHESIS) {
				output->elements.push_back((MathElement){
					component: nullptr,
					specialOp: RIGHT_PARENTHESIS_OPERATOR,
				});
				engine->tokenizer->getToken();
				break; // quit out of loop since our statement is now over
			}
			else { // if we see an end to the statement, quit out
				break;
			}
		}
		else {
			if(engine->tokenizer->peekToken().type == LEFT_PARENTHESIS) { // nested math expression
				output->elements.push_back((MathElement){
					component: Component::AfterParse(MathExpression::Parse(nullptr, output, engine), output, engine),
				});
				expectingOperator = true;
			}
			else if(engine->tokenizer->peekToken().type == LOGICAL_NOT) {
				engine->tokenizer->getToken(); // absorb token
				output->elements.push_back((MathElement){
					component: nullptr,
					specialOp: LOGICAL_NOT_OPERATOR,
				});
				expectingOperator = false;
			}
			else if(engine->tokenizer->peekToken().type == BITWISE_NOT) {
				engine->tokenizer->getToken(); // absorb token
				output->elements.push_back((MathElement){
					component: nullptr,
					specialOp: BITWISE_NOT_OPERATOR,
				});
				expectingOperator = false;
			}
			else if(engine->tokenizer->peekToken().type == MINUS) {
				engine->tokenizer->getToken(); // absorb token
				output->elements.push_back((MathElement){
					component: nullptr,
					specialOp: MINUS_OPERATOR,
				});
				expectingOperator = false;
			}
			// handles literals, accesses, calls, etc
			else if(Component::ShouldParse(output, engine)) {
				output->elements.push_back((MathElement){
					component: Component::Parse(output, engine),
				});
				expectingOperator = true;
			}
			else {
				break; // quit out since we didn't recognize something
			}
		}
	}

	// under this circumstance, we need to check if there is more math expression to parse
	if(parenthesis && parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(output, engine)) {
		return MathExpression::Parse(output, parent, engine);
	}
	else if(parenthesis && output->elements.back().specialOp != RIGHT_PARENTHESIS_OPERATOR) {
		engine->parser->error("unclosed parenthesis");
	}

	if(expectingOperator == false) {
		engine->parser->error("was expecting evaluatable expression, number literal, boolean literal, or string literal");
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
			if(this-engine->parser->minified && lastElement.op.type == MINUS) {
				output += " -";
			}
			else {
				output += "-";
			}
		}
		else {
			if(this->engine->parser->minified) {
				if(element.op.type == SPC || element.op.type == TAB || element.op.type == NL || element.op.type == MODULUS) {
					output += " " + element.op.lexeme + " ";
					continue;
				}
			}
			output += this->engine->parser->space + element.op.lexeme + this->engine->parser->space;
		}
		lastElement = element;
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}

	return output;
}

string MathExpression::printJSON() {
	string output = "{\"type\":\"MATH_EXPRESSION\",\"expression\":[";
	for(MathElement element: this->elements) {
		if(element.component != nullptr) {
			output += element.component->printJSON() + ",";
		}
		else if(element.specialOp == LEFT_PARENTHESIS_OPERATOR) {
			output += "\"(\",";
		}
		else if(element.specialOp == RIGHT_PARENTHESIS_OPERATOR) {
			output += "\")\",";
		}
		else if(element.specialOp == LOGICAL_NOT_OPERATOR) {
			output += "\"!\",";
		}
		else if(element.specialOp == BITWISE_NOT_OPERATOR) {
			output += "\"~\",";
		}
		else if(element.specialOp == MINUS_OPERATOR) {
			output += "\"-\",";
		}
		else {
			output += "\"" + element.op.lexeme + "\",";
		}
	}

	if(output.back() == ',') {
		output.pop_back();
	}

	output += "]}";
	return output;
}

map<TokenType, int> MathExpression::CreatePrecedenceMap() {
	map<TokenType, int> output;
	// use the precedence map for logical operators, even though they aren't
	// handled using the normal postfix/prefix calculations
	output[LOGICAL_OR] = 0;
	output[LOGICAL_AND] = 1;

	// use C-style operator precedence
	output[BITWISE_OR] = 0;
	output[BITWISE_XOR] = 1;
	output[BITWISE_AND] = 2;
	output[EQUAL] = 3;
	output[NOT_EQUAL] = 3;
	output[STRING_EQUAL] = 3;
	output[STRING_NOT_EQUAL] = 3;
	output[LESS_THAN_EQUAL] = 4;
	output[GREATER_THAN_EQUAL] = 4;
	output[LESS_THAN] = 4;
	output[GREATER_THAN] = 4;
	output[SHIFT_LEFT] = 5;
	output[SHIFT_RIGHT] = 5;
	output[PLUS] = 6;
	output[MINUS] = 6;
	output[MODULUS] = 7;
	output[ASTERISK] = 7;
	output[SLASH] = 7;
	return output;
}

ts::instruction::InstructionType MathExpression::TypeToOperator(TokenType type) {
	switch(type) {
		case PLUS:
			return ts::instruction::MATH_ADDITION;
		case MINUS:
			return ts::instruction::MATH_SUBTRACT;
		case ASTERISK:
			return ts::instruction::MATH_MULTIPLY;
		case SLASH:
			return ts::instruction::MATH_DIVISION;
		case MODULUS:
			return ts::instruction::MATH_MODULUS;
		case EQUAL:
			return ts::instruction::MATH_EQUAL;
		case NOT_EQUAL:
			return ts::instruction::MATH_NOT_EQUAL;
		case STRING_EQUAL:
			return ts::instruction::MATH_STRING_EQUAL;
		case STRING_NOT_EQUAL:
			return ts::instruction::MATH_STRING_NOT_EQUAL;
		case LESS_THAN_EQUAL:
			return ts::instruction::MATH_LESS_THAN_EQUAL;
		case GREATER_THAN_EQUAL:
			return ts::instruction::MATH_GREATER_THAN_EQUAL;
		case LESS_THAN:
			return ts::instruction::MATH_LESS_THAN;
		case GREATER_THAN:
			return ts::instruction::MATH_GREATER_THAN;
		case BITWISE_AND:
			return ts::instruction::MATH_BITWISE_AND;
		case BITWISE_OR:
			return ts::instruction::MATH_BITWISE_OR;
		case BITWISE_XOR:
			return ts::instruction::MATH_BITWISE_XOR;
		case SHIFT_LEFT:
			return ts::instruction::MATH_SHIFT_LEFT;
		case SHIFT_RIGHT:
			return ts::instruction::MATH_SHIFT_RIGHT;
		case APPEND:
			return ts::instruction::MATH_APPEND;
		case SPC:
			return ts::instruction::MATH_SPC;
		case TAB:
			return ts::instruction::MATH_TAB;
		case NL:
			return ts::instruction::MATH_NL;

		default:
			return ts::instruction::INVALID_INSTRUCTION;
	}
}

vector<PostfixElement> MathExpression::convertToPostfix(vector<MathElement*>* list, bool prefixMod) {
	vector<PostfixElement> postfix;
	stack<MathElement*> operatorStack;

	if(prefixMod) {
		reverse(list->begin(), list->end());
	}

	MathElement* lastElement = nullptr;
	bool pushedSpecialOp = false;
	for(auto it = list->begin(); it != list->end(); ++it) {
		MathElement* element = *it;

		if(element->specialOp == LEFT_PARENTHESIS_OPERATOR || element->specialOp == RIGHT_PARENTHESIS_OPERATOR) {
			continue;
		}
		
		if(element->component != nullptr) { // if the element is an operand, push it to the stack
			if(pushedSpecialOp) {
				postfix[postfix.size() - 1].element = element;
			}
			else {
				postfix.push_back((PostfixElement){
					element: element
				});

				lastElement = element;
			}
		}
		else if(element->specialOp != INVALID_OPERATOR) {
			if(prefixMod) {
				if(!pushedSpecialOp) {
					postfix.pop_back(); // pop last element, because we're stealing it
					postfix.push_back((PostfixElement){
						element: lastElement,
					});
				}
				postfix[postfix.size() - 1].unary.push_back(element->specialOp);
				pushedSpecialOp = true;
			}
			else {
				if(!pushedSpecialOp) {
					postfix.push_back((PostfixElement){
						element: nullptr,
					});
				}
				postfix[postfix.size() - 1].unary.push_front(element->specialOp);
				pushedSpecialOp = true;
			}
			continue;
		}
		else {
			if(
				operatorStack.size() == 0
				|| MathExpression::Precedence[operatorStack.top()->op.type] < MathExpression::Precedence[element->op.type]
			) {
				operatorStack.push(element);
			}
			else {
				// push operators onto the final stack if the operators on the operator stack are greater precedence than
				// the current operator
				while(
					operatorStack.size() != 0
					&& (
						prefixMod
						? MathExpression::Precedence[operatorStack.top()->op.type] > MathExpression::Precedence[element->op.type]
						: MathExpression::Precedence[operatorStack.top()->op.type] >= MathExpression::Precedence[element->op.type]
					)
				) {
					postfix.push_back((PostfixElement){
						element: operatorStack.top()
					});
					operatorStack.pop();
				}
				operatorStack.push(element);
			}
		}

		pushedSpecialOp = false;
	}

	// push rest of operators onto the stack
	while(operatorStack.size() != 0) {
		postfix.push_back((PostfixElement){
			operatorStack.top()
		});
		operatorStack.pop();
	}

	return postfix;
}

ts::InstructionReturn MathExpression::compileList(vector<MathElement*>* list, ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;
	
	vector<PostfixElement> postfix = this->convertToPostfix(list, TS_INTERPRETER_PREFIX);
	stack<Component*> componentStack;

	struct Value {
		Component* component;
		ts::Instruction* math;
		vector<ts::instruction::UnaryOperator> unary;

		Value(Component* component, ts::Instruction* math) {
			this->component = component;
			this->math = math;
		}
	};

	// generate the ideal instruction execution order
	vector<Value*> instructionList;
	for(PostfixElement &element: postfix) {
		if(element.element->component == nullptr) { // handle an operator
			ts::Instruction* instruction = new ts::Instruction();
			instruction->type = MathExpression::TypeToOperator(element.element->op.type);
			instruction->mathematics.lvalueEntry = ts::Entry();
			instruction->mathematics.lvalueEntry.type = ts::entry::INVALID;
			instruction->mathematics.rvalueEntry = ts::Entry();
			instruction->mathematics.rvalueEntry.type = ts::entry::INVALID;
			instruction->mathematics.lvalueStackIndex = -1;
			instruction->mathematics.rvalueStackIndex = -1;

			instructionList.push_back(new Value(nullptr, instruction)); // push empty value as dummy for our result
		}
		else if(element.unary.size() != 0) {
			Value* value = new Value(element.element->component, nullptr);
			
			for(SpecialOperator unaryOperator: element.unary) {
				switch(unaryOperator) {
					case BITWISE_NOT_OPERATOR: {
						value->unary.push_back(ts::instruction::BITWISE_NOT);
						break;
					}
					
					case LOGICAL_NOT_OPERATOR: {
						value->unary.push_back(ts::instruction::LOGICAL_NOT);
						break;
					}

					case MINUS_OPERATOR: {
						value->unary.push_back(ts::instruction::NEGATE);
						break;
					}

					default: {
						value->unary.push_back(ts::instruction::INVALID_UNARY);
						break;
					}
				}
			}

			instructionList.push_back(value);
		}
		else {
			instructionList.push_back(new Value(element.element->component, nullptr));
		}
	}

	// go through instruction stack and evaluate it, determining which entries we should turn into literals
	stack<Value*> evaluationStack;
	vector<Value*> eraseList;
	for(Value* value: instructionList) {
		if(value->component != nullptr) {
			evaluationStack.push(value);
		}
		else if(value->math != nullptr) {
			Value* lvalue;
			Value* rvalue;
			if(TS_INTERPRETER_PREFIX) {
				lvalue = evaluationStack.top();
				evaluationStack.pop();
				rvalue = evaluationStack.top();
				evaluationStack.pop();
			}
			else {
				rvalue = evaluationStack.top();
				evaluationStack.pop();
				lvalue = evaluationStack.top();
				evaluationStack.pop();
			}

			// figure out if we should cache literal in instruction
			if(lvalue->component != nullptr) {
				if(lvalue->unary.size() == 0 && lvalue->component->getType() == NUMBER_LITERAL) {
					value->math->mathematics.lvalueEntry.setNumber(((NumberLiteral*)lvalue->component)->getNumber());
					eraseList.push_back(lvalue);
				}
				else if(
					lvalue->component->getType() == ACCESS_STATEMENT
					&& ((AccessStatement*)(lvalue->component))->isLocalVariable()
					&& ((AccessStatement*)(lvalue->component))->chainSize() == 1
				) {
					value->math->mathematics.lvalueStackIndex = ((AccessStatement*)(lvalue->component))->getStackIndex(context.scope);
					eraseList.push_back(lvalue);
				}
			}

			if(rvalue->component != nullptr) {
				if(rvalue->unary.size() == 0 && rvalue->component->getType() == NUMBER_LITERAL) {
					value->math->mathematics.rvalueEntry.setNumber(((NumberLiteral*)rvalue->component)->getNumber());
					eraseList.push_back(rvalue);
				}
				else if(
					rvalue->component->getType() == ACCESS_STATEMENT
					&& ((AccessStatement*)(rvalue->component))->isLocalVariable()
					&& ((AccessStatement*)(rvalue->component))->chainSize() == 1
				) {
					value->math->mathematics.rvalueStackIndex = ((AccessStatement*)(rvalue->component))->getStackIndex(context.scope);
					eraseList.push_back(rvalue);
				}
			}
			
			evaluationStack.push(value);
		}
		else {
			evaluationStack.push(value);
		}
	}

	// erase values we swapped for literals
	for(Value* value: eraseList) {
		instructionList.erase(find(instructionList.begin(), instructionList.end(), value));
		delete value;
	}

	// finally add instructions to output
	for(Value* value: instructionList) {
		if(value->component != nullptr) {
			output.add(value->component->compile(engine, context));

			if(value->unary.size() != ts::instruction::INVALID_UNARY) {
				for(ts::instruction::UnaryOperator operation: value->unary) {
					ts::Instruction* unaryInstruction = new ts::Instruction();
					unaryInstruction->type = ts::instruction::UNARY_MATHEMATICS;
					unaryInstruction->unaryMathematics.operation = operation;
					output.add(unaryInstruction);
				}
			}
		}
		else if(value->math != nullptr) {
			output.add(value->math);
		}

		delete value;
	}

	return output;
}

ts::InstructionReturn MathExpression::compile(ts::Engine* engine, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// split along logical operators
	vector<LogicalElement> splitElements;
	vector<MathElement*>* current = new vector<MathElement*>();
	for(MathElement &element: this->elements) {
		if(element.op.type == LOGICAL_OR) {
			LogicalElement value1 = {
				list: current
			};
			splitElements.push_back(value1);

			LogicalElement value2 = {
				list: nullptr,
				op: element.op
			};
			splitElements.push_back(value2);

			current = new vector<MathElement*>();
		}
		else {
			current->push_back(&element);
		}
	}
	
	// push last value
	LogicalElement tempValue = {
		list: current
	};
	splitElements.push_back(tempValue);

	// result of split:
	// 5 || 1 && 6 || 7 => [(5), (||), (1 && 6), (||), (7)]
	ts::Instruction* noop = nullptr;
	vector<MathElement*> andList; // place to temporarily store && operands for compilation
	for(LogicalElement &value: splitElements) {
		if(value.list != nullptr) {
			// parse potential && runs
			ts::Instruction* andNoop = nullptr;
			for(MathElement* element: *value.list) {
				if(element->component != nullptr) {
					andList.push_back(element);
				}
				else if(element->op.type != LOGICAL_AND) {
					andList.push_back(element);
				}
				else {
					output.add(this->compileList(&andList, engine, context));
					andList.clear();

					if(andNoop == nullptr) {
						andNoop = new ts::Instruction();
						andNoop->type = ts::instruction::NOOP;
					}

					ts::Instruction* jumpIfFalse = new ts::Instruction();
					jumpIfFalse->type = ts::instruction::JUMP_IF_FALSE;
					jumpIfFalse->jumpIfFalse.instruction = andNoop;
					jumpIfFalse->jumpIfFalse.pop = false;
					output.add(jumpIfFalse);

					ts::Instruction* pop = new ts::Instruction();
					pop->type = ts::instruction::POP;
					output.add(pop);
				}
			}

			output.add(this->compileList(&andList, engine, context));
			andList.clear();

			output.add(andNoop);
		}
		else { // if we hit an || operator, compile the relevant instructions
			if(noop == nullptr) {
				noop = new ts::Instruction();
				noop->type = ts::instruction::NOOP;
			}
			
			ts::Instruction* jumpIfTrue = new ts::Instruction();
			jumpIfTrue->type = ts::instruction::JUMP_IF_TRUE;
			jumpIfTrue->jumpIfTrue.instruction = noop;
			jumpIfTrue->jumpIfTrue.pop = false;
			output.add(jumpIfTrue);

			ts::Instruction* pop = new ts::Instruction();
			pop->type = ts::instruction::POP;
			output.add(pop);
		}
	}
	output.add(noop);

	delete current;

	return output;
}
