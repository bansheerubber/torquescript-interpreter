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

Component::Component(Parser* parser) {
	this->parser = parser;
}

bool Component::ShouldParse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	return AccessStatement::ShouldParse(tokenizer, parser)
		|| NumberLiteral::ShouldParse(tokenizer, parser)
		|| StringLiteral::ShouldParse(tokenizer, parser)
		|| MathExpression::ShouldParse(nullptr, tokenizer, parser)
		|| BooleanLiteral::ShouldParse(tokenizer, parser)
		|| NewStatement::ShouldParse(tokenizer, parser)
		|| NamespaceStatement::ShouldParse(tokenizer, parser)
		|| InheritanceStatement::ShouldParse(nullptr, parent, tokenizer, parser)
		|| Symbol::ShouldParse(tokenizer, parser);
}

// handles member chaining, inline conditionals. basically, any tacked on stuff that we might
// expect at the end of any component
Component* Component::AfterParse(Component* lvalue, Component* parent, Tokenizer* tokenizer, Parser* parser) {
	if(tokenizer->peekToken().type == MEMBER_CHAIN) { // we have an access statement
		Component* access = AccessStatement::Parse(lvalue, parent, tokenizer, parser);
		
		bool isPostfix = false;
		if(PostfixStatement::ShouldParse(tokenizer, parser)) {
			access = PostfixStatement::Parse((AccessStatement*)access, parent, tokenizer, parser);
			isPostfix = true;
		}

		if(!isPostfix && AssignStatement::ShouldParse((AccessStatement*)access, parent, tokenizer, parser)) {
			lvalue = AssignStatement::Parse((AccessStatement*)access, parent, tokenizer, parser);
		}
		else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(access, tokenizer, parser)) {
			lvalue = MathExpression::Parse(access, parent, tokenizer, parser); // let math expression take over parsing
		}
		else {
			lvalue = access;
		}
	}

	if(lvalue != nullptr && InheritanceStatement::ShouldParse(lvalue, parent, tokenizer, parser)) {
		lvalue = InheritanceStatement::Parse(lvalue, parent, tokenizer, parser);
	}
	else if(InlineConditional::ShouldParse(tokenizer, parser) && lvalue != nullptr && parent->getType() != MATH_EXPRESSION) {
		lvalue = InlineConditional::Parse(lvalue, parent, tokenizer, parser);
	}

	return lvalue;
}

Component* Component::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	Component* output = nullptr;

	if(NamespaceStatement::ShouldParse(tokenizer, parser)) {
		output = NamespaceStatement::Parse(parent, tokenizer, parser);

		if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(output, tokenizer, parser)) {
			output = MathExpression::Parse(output, parent, tokenizer, parser); // let math expression take over parsing
		}
	}
	else if(AccessStatement::ShouldParse(tokenizer, parser)) {
		Component* lvalue = AccessStatement::Parse(nullptr, parent, tokenizer, parser);
		
		bool isPostfix = false;
		if(PostfixStatement::ShouldParse(tokenizer, parser)) {
			lvalue = PostfixStatement::Parse((AccessStatement*)lvalue, parent, tokenizer, parser);
			isPostfix = true;
		}
		// handle inline conditionals here
		else if(InlineConditional::ShouldParse(tokenizer, parser) && lvalue != nullptr) {
			lvalue = InlineConditional::Parse(lvalue, parent, tokenizer, parser);
		}

		if(!isPostfix && AssignStatement::ShouldParse((AccessStatement*)lvalue, parent, tokenizer, parser)) {
			output = AssignStatement::Parse((AccessStatement*)lvalue, parent, tokenizer, parser);
		}
		else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(lvalue, tokenizer, parser)) {
			output = MathExpression::Parse(lvalue, parent, tokenizer, parser); // let math expression take over parsing
		}
		else {
			output = lvalue;
		}
	}
	else if(parent->getType() != MATH_EXPRESSION && MathExpression::ShouldParse(nullptr, tokenizer, parser)) {
		output = MathExpression::Parse(nullptr, parent, tokenizer, parser);
	}
	else if(NumberLiteral::ShouldParse(tokenizer, parser)) {
		output = NumberLiteral::Parse(parent, tokenizer, parser);
	}
	else if(StringLiteral::ShouldParse(tokenizer, parser)) {
		output = StringLiteral::Parse(parent, tokenizer, parser);
	}
	else if(BooleanLiteral::ShouldParse(tokenizer, parser)) {
		output = BooleanLiteral::Parse(parent, tokenizer, parser);
	}
	else if(NewStatement::ShouldParse(tokenizer, parser)) {
		output = NewStatement::Parse(parent, tokenizer, parser);
	}
	else if(InheritanceStatement::ShouldParse(nullptr, parent, tokenizer, parser)) {
		output = InheritanceStatement::Parse(nullptr, parent, tokenizer, parser);
	}
	else if(Symbol::ShouldParse(tokenizer, parser)) {
		output = Symbol::Parse(parent, tokenizer, parser);
	}
	
	// additional support for edge cases
	return Component::AfterParse(output, parent, tokenizer, parser);
}

// handle recursive body parsing
void Component::ParseBody(Body* body, Tokenizer* tokenizer, Parser* parser, bool oneLine) {
	int count = 0;
	while(!tokenizer->eof()) {
		if(oneLine && count > 0) {
			break;
		}

		if(Component::ShouldParse(body, tokenizer, parser)) {
			body->addChild(Component::Parse(body, tokenizer, parser));
			Token token = tokenizer->peekToken();
			if(token.type == SEMICOLON) {
				tokenizer->getToken(); // absorb the semicolon
			}
			// see if we have an invalid lvalue
			else if(AssignStatement::ShouldParse(nullptr, body, tokenizer, parser)) {
				parser->error("invalid lvalue for assignment");
			}
			// see if unary operator is being weird
			else if(PostfixStatement::ShouldParse(tokenizer, parser)) {
				parser->error("invalid unary postfix operator");
			}
			else {
				parser->error("no semicolon at end of line");
			}
		}
		else if(Comment::ShouldParse(tokenizer, parser)) {
			body->addChild(Comment::Parse(body, tokenizer, parser));
		}
		else if(IfBody::ShouldParse(tokenizer, parser)) {
			body->addChild(IfBody::Parse(body, tokenizer, parser));
		}
		else if(FunctionDeclaration::ShouldParse(tokenizer, parser)) {
			body->addChild(FunctionDeclaration::Parse(body, tokenizer, parser));
		}
		else if(ReturnStatement::ShouldParse(tokenizer, parser)) {
			body->addChild(ReturnStatement::Parse(body, tokenizer, parser));
		}
		else if(DatablockDeclaration::ShouldParse(tokenizer, parser)) {
			body->addChild(DatablockDeclaration::Parse(body, tokenizer, parser));
		}
		else if(ForBody::ShouldParse(tokenizer, parser)) {
			body->addChild(ForBody::Parse(body, tokenizer, parser));
		}
		else if(WhileBody::ShouldParse(tokenizer, parser)) {
			body->addChild(WhileBody::Parse(body, tokenizer, parser));
		}
		else if(SwitchBody::ShouldParse(tokenizer, parser)) {
			body->addChild(SwitchBody::Parse(body, tokenizer, parser));
		}
		else if(PackageDeclaration::ShouldParse(tokenizer, parser)) {
			body->addChild(PackageDeclaration::Parse(body, tokenizer, parser));
		}
		else if(ContinueStatement::ShouldParse(tokenizer, parser)) {
			body->addChild(ContinueStatement::Parse(body, tokenizer, parser));
		}
		else if(BreakStatement::ShouldParse(tokenizer, parser)) {
			body->addChild(BreakStatement::Parse(body, tokenizer, parser));
		}
		// error if we get something we don't recognize
		else if(
			tokenizer->peekToken().type != SEMICOLON
			&& tokenizer->peekToken().type != RIGHT_BRACKET
			&& tokenizer->peekToken().type != CASE
			&& tokenizer->peekToken().type != DEFAULT
		) {
			parser->error("unexpected token '%s'", tokenizer->peekToken().lexeme.c_str());
		}
		else {
			break;
		}

		count++;
	}
}