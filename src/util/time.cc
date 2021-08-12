#include "time.h"

#include <chrono>

using namespace std;

unsigned long long getMicrosecondsNow() {
	return chrono::duration_cast<chrono::microseconds>(
		chrono::high_resolution_clock::now().time_since_epoch()
	).count();
}
