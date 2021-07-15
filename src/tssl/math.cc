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
		void* mAbs(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				double number = *((double*)args[0]);
				if(number < 0) {
					return new double(-number);
				}
				else {
					return new double(number);
				}
			}
			return new double(0.0);
		}

		void* mACos(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(acos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mASin(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(asin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mATan(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 2) {
				return new double(atan2(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mCeil(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(ceil(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloor(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				double number = *((double*)args[0]);
				if(number < 0) {
					return new double((int)(number - 1));
				}
				else {
					return new double((int)number);
				}
			}
			return new double(0.0);
		}

		void* mCos(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(cos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mSin(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(sin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mTan(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(tan(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mPow(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 2) {
				return new double(pow(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mSqrt(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(sqrt(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mDegToRad(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * M_PI / 180.0);
			}
			return new double(0.0);
		}

		void* mRadToDeg(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * 180.0 / M_PI);
			}
			return new double(0.0);
		}

		void* mLog(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				return new double(log(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloatLength(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 2) {
				int precision = (int)(*((double*)args[1]));
				if(precision < 0) {
					precision = 0;
				}
				else if(precision > 16) {
					precision = 16;
				}

				string formatter("{:.");
				formatter += std::to_string(precision);
				formatter += "f}";
				string formatted(fmt::format(formatter, *((double*)args[0])));
				return stringToChars(formatted);
			}
			return getEmptyString();
		}
	}
}