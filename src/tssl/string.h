#pragma once

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

namespace ts {
	class Engine;

	namespace sl {
		Entry* strLen(Engine* engine, unsigned int argc, Entry* args);
		Entry* getSubStr(Engine* engine, unsigned int argc, Entry* args);
		Entry* strPos(Engine* engine, unsigned int argc, Entry* args);
		Entry* trim(Engine* engine, unsigned int argc, Entry* args);
		Entry* ltrim(Engine* engine, unsigned int argc, Entry* args);
		Entry* rtrim(Engine* engine, unsigned int argc, Entry* args);
		Entry* strCmp(Engine* engine, unsigned int argc, Entry* args);
		Entry* strICmp(Engine* engine, unsigned int argc, Entry* args);
		Entry* strLwr(Engine* engine, unsigned int argc, Entry* args);
		Entry* strUpr(Engine* engine, unsigned int argc, Entry* args);
		Entry* strChr(Engine* engine, unsigned int argc, Entry* args);
		Entry* strStr(Engine* engine, unsigned int argc, Entry* args);
		Entry* stripChars(Engine* engine, unsigned int argc, Entry* args);
		Entry* _collapseEscape(Engine* engine, unsigned int argc, Entry* args);
		Entry* expandEscape(Engine* engine, unsigned int argc, Entry* args);
		Entry* strReplace(Engine* engine, unsigned int argc, Entry* args);
	}
}
