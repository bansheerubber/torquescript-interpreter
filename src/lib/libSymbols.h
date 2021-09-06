#pragma once

extern "C" {	
	typedef void* tsEngine;

	typedef struct tsObjectWrapper {
		void* object;
		void* customData;
	} tsObjectWrapper;
	
	tsEngine tsCreateEngine(char isParallel);

	bool tsTick(tsEngine engine);
	void tsSetTickRate(tsEngine engine, long tickRate);
	void tsExecFile(tsEngine engine, const char* filename);
}
