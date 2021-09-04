#include "engine.h"

#include "../compiler/compiler.h"
#include "../interpreter/instruction.h"
#include "../components/sourceFile.h"

using namespace ts;

Engine::Engine(ParsedArguments args) {
	this->args = args;
	this->tokenizer = new Tokenizer(this, args);
	this->parser = new Parser(this, args);
	this->interpreter = new Interpreter(this, args, false);
}

Engine::~Engine() {
	delete this->tokenizer;
	delete this->parser;
	delete this->interpreter;
}

void Engine::interpretFile(string fileName) {
	this->tokenizer->tokenizeFile(fileName);
	this->parser->startParse();

	// compile
	InstructionReturn result = parser->getSourceFile()->compile(this, {
		loop: nullptr,
		scope: nullptr,
	});

	this->interpreter->startInterpretation(result.first);
}

void Engine::interpretPiped(string piped) {
	this->tokenizer->tokenizePiped(piped);
	this->parser->startParse();

	// compile
	InstructionReturn result = parser->getSourceFile()->compile(this, {
		loop: nullptr,
		scope: nullptr,
	});

	this->interpreter->startInterpretation(result.first);	
}
