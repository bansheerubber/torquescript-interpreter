#include <stdio.h>

#include <mutex>

#include "../engine/engine.h"
#include "../io.h"
#include "parser.h"

std::mutex& errorLock() {
	static std::mutex m;
	return m;
}

void Parser::error(const char* format, ...) {
	lock_guard<mutex> lock(errorLock());
	
	Token token = this->engine->tokenizer->peekToken();
	printError("%s:%d:%d: ", this->engine->tokenizer->fileName.c_str(), token.lineNumber, token.characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	printError(format, argptr);
	va_end(argptr);

	printError("\n");

	exit(1);
}

void Parser::warning(const char* format, ...) {
	lock_guard<mutex> lock(errorLock());
	
	Token token = this->engine->tokenizer->peekToken();
	printWarning("%s:%d:%d: ", this->engine->tokenizer->fileName.c_str(), token.lineNumber, token.characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	printWarning(format, argptr);
	va_end(argptr);

	printWarning("\n");
}

## parser_debug.py

const char* Parser::typeToName(ComponentType type) {
	return ComponentTypeDebug[type];
}