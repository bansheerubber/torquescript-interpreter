#include "packageDeclaration.h"
#include "../interpreter/interpreter.h"

bool PackageDeclaration::ShouldParse(ts::Engine* engine) {
	return engine->tokenizer->peekToken().type == PACKAGE;
}

PackageDeclaration* PackageDeclaration::Parse(Component* parent, ts::Engine* engine) {
	PackageDeclaration* output = new PackageDeclaration(engine);

	engine->parser->expectToken(PACKAGE);

	// parse a symbol
	if(!Symbol::ShouldParse(engine)) {
		engine->parser->error("invalid package name");
	}
	output->packageName = Symbol::Parse(output, engine);

	engine->parser->expectToken(LEFT_BRACKET);

	Component::ParseBody(output, engine);

	// make sure our children are functions
	for(Component* child: output->children) {
		if(child->getType() != FUNCTION_DECLARATION) {
			engine->parser->error("only functions allowed inside of package");
		}
	}

	engine->parser->expectToken(RIGHT_BRACKET);
	engine->parser->expectToken(SEMICOLON);

	return output;
}

string PackageDeclaration::print() {
	string output = "package " + this->packageName->print() + this->engine->parser->space + "{" + this->engine->parser->newLine;
	output += this->printBody();
	output += "};" + this->engine->parser->newLine;
	return output;
}

string PackageDeclaration::printJSON() {
	return "{\"type\":\"PACKAGE_DECLARATION\",\"name\":" + this->packageName->printJSON() + ",\"body\":" + this->printJSONBody() + "}";
}

ts::InstructionReturn PackageDeclaration::compile(ts::Engine* engine, ts::CompilationContext context) {
	for(Component* component: this->children) {
		component->compile(engine, (ts::CompilationContext){
			loop: nullptr,
			package: this,
			scope: nullptr,
		});
	}
	
	return {};
}
