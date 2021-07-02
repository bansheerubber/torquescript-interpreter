#pragma once

// some windows horseshit
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

#define FMT_HEADER_ONLY
#include "../include/fmt/include/fmt/format.h"
#include <string>

#include "../util/getEmptyString.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		void* mAbs(size_t argc, void** args) {
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

		void* mACos(size_t argc, void** args) {
			if(argc == 1) {
				return new double(acos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mASin(size_t argc, void** args) {
			if(argc == 1) {
				return new double(asin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mATan(size_t argc, void** args) {
			if(argc == 2) {
				return new double(atan2(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mCeil(size_t argc, void** args) {
			if(argc == 1) {
				return new double(ceil(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloor(size_t argc, void** args) {
			if(argc == 1) {
				return new double(floor(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mCos(size_t argc, void** args) {
			if(argc == 1) {
				return new double(cos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mSin(size_t argc, void** args) {
			if(argc == 1) {
				return new double(sin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mTan(size_t argc, void** args) {
			if(argc == 1) {
				return new double(tan(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mPow(size_t argc, void** args) {
			if(argc == 2) {
				return new double(pow(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mSqrt(size_t argc, void** args) {
			if(argc == 1) {
				return new double(sqrt(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mDegToRad(size_t argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * M_PI / 180.0);
			}
			return new double(0.0);
		}

		void* mRadToDeg(size_t argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * 180.0 / M_PI);
			}
			return new double(0.0);
		}

		void* mLog(size_t argc, void** args) {
			if(argc == 1) {
				return new double(log(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloatLength(size_t argc, void** args) {
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