#include "math.h"

// some windows horseshit
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

#define FMT_HEADER_ONLY
#include "../include/fmt/include/fmt/format.h"
#include <string>

#include "../util/getEmptyString.h"
#include "../interpreter/interpreter.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		Entry* mAbs(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				double number = args[0].numberData;
				if(number < 0) {
					return new Entry(-number);
				}
				else {
					return new Entry(number);
				}
			}
			return new Entry(0.0);
		}

		Entry* mACos(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(acos(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mASin(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(asin(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mATan(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				return new Entry(atan2(args[0].numberData, args[1].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mCeil(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(ceil(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mFloor(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				double number = args[0].numberData;
				if(number < 0) {
					return new Entry((int)(number - 1));
				}
				else {
					return new Entry((int)number);
				}
			}
			return new Entry(0.0);
		}

		Entry* mCos(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(cos(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mSin(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(sin(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mTan(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(tan(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mPow(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				return new Entry(pow(args[0].numberData, args[1].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mSqrt(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(sqrt(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mDegToRad(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(args[0].numberData * M_PI / 180.0);
			}
			return new Entry(0.0);
		}

		Entry* mRadToDeg(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(args[0].numberData * 180.0 / M_PI);
			}
			return new Entry(0.0);
		}

		Entry* mLog(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(log(args[0].numberData));
			}
			return new Entry(0.0);
		}

		Entry* mFloatLength(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				int precision = (int)args[1].numberData;
				if(precision < 0) {
					precision = 0;
				}
				else if(precision > 16) {
					precision = 16;
				}

				string formatter("{:.");
				formatter += std::to_string(precision);
				formatter += "f}";
				string formatted(fmt::format(formatter, args[0].numberData));
				return new Entry(stringToChars(formatted));
			}
			return nullptr;
		}
	}
}