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
	(*this->engine->errorFunction)("%s:%d:%d: ", this->engine->tokenizer->fileName.c_str(), token.lineNumber, token.characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	(*this->engine->vErrorFunction)(format, argptr);
	va_end(argptr);

	(*this->engine->errorFunction)("\n");

	exit(1);
}

void Parser::warning(const char* format, ...) {
	lock_guard<mutex> lock(errorLock());
	
	Token token = this->engine->tokenizer->peekToken();
	(*this->engine->warningFunction)("%s:%d:%d: ", this->engine->tokenizer->fileName.c_str(), token.lineNumber, token.characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	(*this->engine->vWarningFunction)(format, argptr);
	va_end(argptr);

	(*this->engine->warningFunction)("\n");
}

## parser_debug.py

const char* Parser::typeToName(ComponentType type) {
	return ComponentTypeDebug[type];
}