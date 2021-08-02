#pragma once

#include <string>

#include "entry.h"

using namespace std;

namespace ts {
	struct Schedule {
		unsigned long long time; // how long this schedule should be
		unsigned long long start; // when this schedule started
		unsigned long long end; // when the schedule should end
		string command;
		Entry* arguments;
		size_t argumentCount;
		ObjectReference* object;

		Schedule(
			unsigned long long time,
			unsigned long long start,
			string command,
			Entry* arguments,
			size_t argumentCount,
			class ObjectReference* object = nullptr
		) {
			this->time = time;
			this->start = start;
			this->command = command;
			this->arguments = arguments;
			this->argumentCount = argumentCount;
			this->object = object;

			this->end = time + start;
		}

		bool operator<(const Schedule &other) {
			return this->end < other.end;
		}

		bool operator>(const Schedule &other) {
			return this->end > other.end;
		}
	};
}
