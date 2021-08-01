#pragma once

#include <string>

#include "entry.h"

using namespace std;

namespace ts {
	struct Schedule {
		unsigned long long time; // how long this schedule should be
		unsigned long long start; // when this schedule started
		string command;
		Entry* arguments;
		size_t argumentCount;

		Schedule(unsigned long long time, unsigned long long start, string command, Entry* arguments, size_t argumentCount) {
			this->time = time;
			this->start = start;
			this->command = command;
			this->arguments = arguments;
			this->argumentCount = argumentCount;
		}

		bool operator<(const Schedule &other) {
			return this->start < other.start;
		}
	};
}
