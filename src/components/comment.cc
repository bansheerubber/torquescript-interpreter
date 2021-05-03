#include "comment.h"

bool Comment::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == COMMENT;
}

Comment* Comment::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	Comment* output = new Comment(parser);
	output->parent = parent;
	output->comment = tokenizer->getToken().lexeme;
	return output;
}

string Comment::print() {
	return "//" + this->comment + "\n";
}

ts::InstructionReturn Comment::compile() {
	return {};
}
