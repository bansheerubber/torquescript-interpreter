#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Interpreter;

	namespace sl {
		Entry* strLen(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* getSubStr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strPos(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* trim(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* ltrim(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* rtrim(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strCmp(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strICmp(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strLwr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strUpr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strChr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strStr(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* stripChars(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* _collapseEscape(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* expandEscape(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* strReplace(Interpreter* interpreter, size_t argc, Entry* args);
	}
}
