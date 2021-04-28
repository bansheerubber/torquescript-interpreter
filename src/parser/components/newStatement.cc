#include "newStatement.h"

bool NewStatement::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return (
		tokenizer->peekToken().type == NEW
		&& (
			tokenizer->peekToken(1).type == SYMBOL
			|| tokenizer->peekToken(1).type == LEFT_PARENTHESIS
		)
	);
}

NewStatement* NewStatement::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	NewStatement* output = new NewStatement(parser);
	output->parent = parent;

	parser->expectToken(NEW);

	// parse a symbol
	if(Symbol::ShouldParse(tokenizer, parser)) {
		output->className = Symbol::Parse(output, tokenizer, parser);
	}
	else if(MathExpression::ShouldParse(nullptr, tokenizer, parser)) {
		output->className = MathExpression::Parse(nullptr, output, tokenizer, parser);
	}
	else {
		parser->error("invalid new object name");
	}

	// parse call list
	if(InheritanceStatement::ShouldParse(tokenizer, parser)) {
		output->arguments = InheritanceStatement::Parse(parent, tokenizer, parser);
	}
	else if(CallStatement::ShouldParse(tokenizer, parser)) {
		output->arguments = CallStatement::Parse(parent, tokenizer, parser);
	}
	else {
		parser->error("invalid new object argument list");
	}

	// expect something not a left bracket if we got no arguments in the body of the new object statement
	if(tokenizer->peekToken().type != LEFT_BRACKET) {
		return output;
	}

	parser->expectToken(LEFT_BRACKET);

	// parse property declaration
	while(!tokenizer->eof()) {
		// new statements can be nested, apparently
		if(NewStatement::ShouldParse(tokenizer, parser)) {
			output->children.push_back(NewStatement::Parse(output, tokenizer, parser));
			parser->expectToken(SEMICOLON);
		}
		else if(AccessStatement::ShouldParse(tokenizer, parser, true)) {
			AccessStatement* access = AccessStatement::Parse(nullptr, output, tokenizer, parser, true);
			if(
				access->hasChain()
				|| access->hasCall()
				|| access->chainSize() > 2
				|| access->isLocalVariable()
				|| access->isGlobalVariable()
			) {
				parser->error("did not expect complex property assignment '%s' in new object", access->print().c_str());
			}

			// now parse the assign statement
			if(!AssignStatement::ShouldParse(access, output, tokenizer, parser)) {
				parser->error("expected property assignment in new object");
			}
			output->children.push_back(AssignStatement::Parse(access, output, tokenizer, parser));

			parser->expectToken(SEMICOLON);
		}
		else if(tokenizer->peekToken().type == RIGHT_BRACKET) {
			break;
		}
		else {
			parser->error("expected property assignment in new object");
		}
	}

	parser->expectToken(RIGHT_BRACKET);

	return output;
}

string NewStatement::print() {
	string output = "new " + this->className->print() + this->arguments->print();
	if(this->children.size() != 0) {
		output += " {" + this->parser->newLine;
		output += this->printBody();
		output += "}";
	}

	if(this->parent->requiresSemicolon(this)) {
		output += ";";
	}
	return output;
}