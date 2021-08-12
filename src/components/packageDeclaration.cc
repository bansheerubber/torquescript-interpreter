#include "packageDeclaration.h"
#include "../interpreter/interpreter.h"

bool PackageDeclaration::ShouldParse(Tokenizer* tokenizer, Parser* parser) {
	return tokenizer->peekToken().type == PACKAGE;
}

PackageDeclaration* PackageDeclaration::Parse(Component* parent, Tokenizer* tokenizer, Parser* parser) {
	PackageDeclaration* output = new PackageDeclaration(parser);

	parser->expectToken(PACKAGE);

	// parse a symbol
	if(!Symbol::ShouldParse(tokenizer, parser)) {
		parser->error("invalid package name");
	}
	output->packageName = Symbol::Parse(output, tokenizer, parser);

	parser->expectToken(LEFT_BRACKET);

	Component::ParseBody(output, tokenizer, parser);

	// make sure our children are functions
	for(Component* child: output->children) {
		if(child->getType() != FUNCTION_DECLARATION) {
			parser->error("only functions allowed inside of package");
		}
	}

	parser->expectToken(RIGHT_BRACKET);
	parser->expectToken(SEMICOLON);

	return output;
}

string PackageDeclaration::print() {
	string output = "package " + this->packageName->print() + this->parser->space + "{" + this->parser->newLine;
	output += this->printBody();
	output += "};" + this->parser->newLine;
	return output;
}

string PackageDeclaration::printJSON() {
	return "{\"type\":\"PACKAGE_DECLARATION\",\"name\":" + this->packageName->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn PackageDeclaration::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	for(Component* component: this->children) {
		component->compile(interpreter, (ts::CompilationContext){
			loop: nullptr,
			package: this,
			scope: nullptr,
		});
	}
	
	return {};
}
