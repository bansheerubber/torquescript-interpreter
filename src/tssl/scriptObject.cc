#include "scriptObject.h"

using namespace ts;

void* sl::ScriptObject__test(int argc, void** args) {
	if(argc >= 1) {
		printf("script object says: %s\n", ((string*)args[1])->c_str());
	}
	return nullptr;
}