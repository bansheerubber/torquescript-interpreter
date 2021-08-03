#pragma once

extern "C" {	
	typedef struct tsInterpreter {
		const unsigned long id;
		const int isParallel;
	} tsInterpreter;
	
	tsInterpreter tsCreateInterpreter(char isParallel);

	void tsTick(tsInterpreter interpreter);
	void tsSetTickRate(tsInterpreter interpreter, long tickRate);
	void tsExecFile(tsInterpreter interpreter, const char* filename);
}
