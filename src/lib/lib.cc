#include "libSymbols.h"

#include <stdio.h>
#include <string>

#include "../args.h"
#include "../compiler/compiler.h"
#include "../interpreter/interpreter.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

void initInterpreterId(void* nothing, ts::Interpreter** value) {
	*value = nullptr;
};

DynamicArray<ts::Interpreter*, void> interpreters(nullptr, 5, initInterpreterId, nullptr);

tsInterpreter tsCreateInterpreter(char isParallel) {
	ParsedArguments args = {};
	ts::Interpreter* interpreter = new ts::Interpreter(args);

	size_t index = interpreters.head;
	interpreters[index] = interpreter;
	interpreters.pushed();
	return {
		id: index,
		isParallel: isParallel,
	};
}

void tsTick(tsInterpreter interpreter) {
	
}

void tsExecFile(tsInterpreter interpreter, const char* filename) {
	interpreters[interpreter.id]->execFile(string(filename));
}