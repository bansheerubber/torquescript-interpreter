#include "comment.h"
#include "../interpreter/interpreter.h"

bool Comment::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == COMMENT;
}

Comment* Comment::Parse(Component* parent, ts::Engine* engine) {
	Comment* output = new Comment(engine);
	output->parent = parent;
	output->comment = engine->tokenizer->getToken().lexeme;
	return output;
}

string Comment::print() {
	return "//" + this->comment + "\n";
}

string Comment::printJSON() {
	return "{\"type\":\"COMMENT_STATEMENT\",\"value\":\"" + this->comment + "\"}";
}

ts::InstructionReturn Comment::compile(ts::Engine* engine, ts::CompilationContext context) {
	return {};
}
