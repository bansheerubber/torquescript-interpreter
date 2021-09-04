#include "component.h"
#include "body.h"
#include "../parser/parser.h"

#include "accessStatement.h"
#include "assignStatement.h"
#include "booleanLiteral.h"
#include "breakStatement.h"
#include "comment.h"
#include "continueStatement.h"
#include "datablockDeclaration.h"
#include "forBody.h"
#include "functionDeclaration.h"
#include "ifBody.h"
#include "inlineConditional.h"
#include "mathExpression.h"
#include "namespaceStatement.h"
#include "newStatement.h"
#include "numberLiteral.h"
#include "packageDeclaration.h"
#include "postfixStatement.h"
#include "returnStatement.h"
#include "stringLiteral.h"
#include "symbol.h"
#include "switchBody.h"
#include "whileBody.h"

Component::Component(ts::Engine* engine) {
	this->engine = engine;
	engine->parser->components.push_back(this);
}

bool Component::ShouldParse(Component* parent, ts::Engine* engine) {
	return AccessStatement::ShouldParse(engine)
		|| NumberLiteral::ShouldParse(engine)
		|| StringLiteral::ShouldParse(engine)
		|| MathExpression::ShouldParse(nullptr, engine)
		|| BooleanLiteral::ShouldParse(engine)
		|| NewStatement::ShouldParse(engine)
		|| NamespaceStatement::ShouldParse(engine)
		|| InheritanceStatement::ShouldParse(nullptr, parent, engine)
		|| Symbol::ShouldParse(engine);
}

// handles member chaining, inline conditionals. basically, any tacked on stuff that we might
// expect at the end of any component
Component* Component::AfterParse(Component* lvalue, Component* parent, ts::Engine* engine) {
	if(engine->tokenizer->peekToken().type == MEMBER_CHAIN) { // we have an access statement
		Component* access = AccessStatement::Parse(lvalue, parent, engine);
		
		bool isPostfix = false;
		if(PostfixStatement::ShouldParse(engine)) {
			access = PostfixStatement::Parse((AccessStatement*)access, parent, engine);
			isPostfix = true;
		}

		if(!isPostfix && AssignStatement::ShouldParse((AccessStatement*)access, parent, engine)) {
			lvalue = AssignStatement::Parse((AccessStatement*)access, parent, engine);
		}
		else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(access, engine)) {
			lvalue = MathExpression::Parse(access, parent, engine); // let math expression take over parsing
		}
		else {
			lvalue = access;
		}
	}

	if(lvalue != nullptr && InheritanceStatement::ShouldParse(lvalue, parent, engine)) {
		lvalue = InheritanceStatement::Parse(lvalue, parent, engine);
	}
	else if(InlineConditional::ShouldParse(engine) && lvalue != nullptr && parent->getType() != MATH_EXPRESSION) {
		lvalue = InlineConditional::Parse(lvalue, parent, engine);
	}

	return lvalue;
}

Component* Component::Parse(Component* parent, ts::Engine* engine) {
	Component* output = nullptr;

	if(NamespaceStatement::ShouldParse(engine)) {
		output = NamespaceStatement::Parse(parent, engine);

		if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(output, engine)) {
			output = MathExpression::Parse(output, parent, engine); // let math expression take over parsing
		}
	}
	else if(AccessStatement::ShouldParse(engine)) {
		Component* lvalue = AccessStatement::Parse(nullptr, parent, engine);
		
		bool isPostfix = false;
		if(PostfixStatement::ShouldParse(engine)) {
			lvalue = PostfixStatement::Parse((AccessStatement*)lvalue, parent, engine);
			isPostfix = true;
		}
		// handle inline conditionals here
		else if(InlineConditional::ShouldParse(engine) && lvalue != nullptr) {
			lvalue = InlineConditional::Parse(lvalue, parent, engine);
		}

		if(!isPostfix && AssignStatement::ShouldParse((AccessStatement*)lvalue, parent, engine)) {
			output = AssignStatement::Parse((AccessStatement*)lvalue, parent, engine);
		}
		else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(lvalue, engine)) {
			output = MathExpression::Parse(lvalue, parent, engine); // let math expression take over parsing
		}
		else {
			output = lvalue;
		}
	}
	else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(nullptr, engine)) {
		output = MathExpression::Parse(nullptr, parent, engine);
	}
	else if(NumberLiteral::ShouldParse(engine)) {
		output = NumberLiteral::Parse(parent, engine);
	}
	else if(StringLiteral::ShouldParse(engine)) {
		output = StringLiteral::Parse(parent, engine);
	}
	else if(BooleanLiteral::ShouldParse(engine)) {
		output = BooleanLiteral::Parse(parent, engine);
	}
	else if(NewStatement::ShouldParse(engine)) {
		output = NewStatement::Parse(parent, engine);
	}
	else if(InheritanceStatement::ShouldParse(nullptr, parent, engine)) {
		output = InheritanceStatement::Parse(nullptr, parent, engine);
	}
	else if(Symbol::ShouldParse(engine)) {
		output = Symbol::Parse(parent, engine);
	}
	
	// additional support for edge cases
	return Component::AfterParse(output, parent, engine);
}

// handle recursive body parsing
void Component::ParseBody(Body* body, ts::Engine* engine, bool oneLine) {
	int count = 0;
	while(!engine->tokenizer->eof()) {
		if(oneLine && count > 0) {
			break;
		}

		if(Component::ShouldParse(body, engine)) {
			body->addChild(Component::Parse(body, engine));
			Token token = engine->tokenizer->peekToken();
			if(token.type == SEMICOLON) {
				engine->tokenizer->getToken(); // absorb the semicolon
			}
			// see if we have an invalid lvalue
			else if(AssignStatement::ShouldParse(nullptr, body, engine)) {
				engine->parser->error("invalid lvalue for assignment");
			}
			// see if unary operator is being weird
			else if(PostfixStatement::ShouldParse(engine)) {
				engine->parser->error("invalid unary postfix operator");
			}
			else {
				engine->parser->error("no semicolon at end of line");
			}
		}
		else if(Comment::ShouldParse(engine)) {
			body->addChild(Comment::Parse(body, engine));
		}
		else if(IfBody::ShouldParse(engine)) {
			body->addChild(IfBody::Parse(body, engine));
		}
		else if(FunctionDeclaration::ShouldParse(engine)) {
			body->addChild(FunctionDeclaration::Parse(body, engine));
		}
		else if(ReturnStatement::ShouldParse(engine)) {
			body->addChild(ReturnStatement::Parse(body, engine));
		}
		else if(DatablockDeclaration::ShouldParse(engine)) {
			body->addChild(DatablockDeclaration::Parse(body, engine));
		}
		else if(ForBody::ShouldParse(engine)) {
			body->addChild(ForBody::Parse(body, engine));
		}
		else if(WhileBody::ShouldParse(engine)) {
			body->addChild(WhileBody::Parse(body, engine));
		}
		else if(SwitchBody::ShouldParse(engine)) {
			body->addChild(SwitchBody::Parse(body, engine));
		}
		else if(PackageDeclaration::ShouldParse(engine)) {
			body->addChild(PackageDeclaration::Parse(body, engine));
		}
		else if(ContinueStatement::ShouldParse(engine)) {
			body->addChild(ContinueStatement::Parse(body, engine));
		}
		else if(BreakStatement::ShouldParse(engine)) {
			body->addChild(BreakStatement::Parse(body, engine));
		}
		// error if we get something we don't recognize
		else if(
			engine->tokenizer->peekToken().type != SEMICOLON
			&& engine->tokenizer->peekToken().type != RIGHT_BRACKET
			&& engine->tokenizer->peekToken().type != CASE
			&& engine->tokenizer->peekToken().type != DEFAULT
			&& engine->tokenizer->peekToken().type != END_OF_FILE
		) {
			engine->parser->error("unexpected token '%s'", engine->tokenizer->peekToken().lexeme.c_str());
		}
		else {
			break;
		}

		count++;
	}
}
