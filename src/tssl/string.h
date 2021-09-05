#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Engine;

	namespace sl {
		Entry* strLen(Engine* engine, size_t argc, Entry* args);
		Entry* getSubStr(Engine* engine, size_t argc, Entry* args);
		Entry* strPos(Engine* engine, size_t argc, Entry* args);
		Entry* trim(Engine* engine, size_t argc, Entry* args);
		Entry* ltrim(Engine* engine, size_t argc, Entry* args);
		Entry* rtrim(Engine* engine, size_t argc, Entry* args);
		Entry* strCmp(Engine* engine, size_t argc, Entry* args);
		Entry* strICmp(Engine* engine, size_t argc, Entry* args);
		Entry* strLwr(Engine* engine, size_t argc, Entry* args);
		Entry* strUpr(Engine* engine, size_t argc, Entry* args);
		Entry* strChr(Engine* engine, size_t argc, Entry* args);
		Entry* strStr(Engine* engine, size_t argc, Entry* args);
		Entry* stripChars(Engine* engine, size_t argc, Entry* args);
		Entry* _collapseEscape(Engine* engine, size_t argc, Entry* args);
		Entry* expandEscape(Engine* engine, size_t argc, Entry* args);
		Entry* strReplace(Engine* engine, size_t argc, Entry* args);
	}
}
