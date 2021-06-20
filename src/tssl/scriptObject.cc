#include "scriptObject.h"

using namespace ts;

void* sl::ScriptObject__test(int argc, void** args) {
	printf("called on script object\n");
	return nullptr;
}