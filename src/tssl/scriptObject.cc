#include "scriptObject.h"

using namespace ts;

void* sl::ScriptObject__test(size_t argc, void** args) {
	if(argc >= 2) {
		printf("attemtping print because argc high\n");
		printf("script object says: %s\n", ((const char*)args[1]));
	}
	return nullptr;
}