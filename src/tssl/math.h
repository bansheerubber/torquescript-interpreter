#pragma once

#include <fmt/core.h>
#include <string>

using namespace std;

namespace ts {
	namespace sl {
		void* mAbs(int argc, void** args) {
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

		void* mACos(int argc, void** args) {
			if(argc == 1) {
				return new double(acos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mASin(int argc, void** args) {
			if(argc == 1) {
				return new double(asin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mATan(int argc, void** args) {
			if(argc == 2) {
				return new double(atan2(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mCeil(int argc, void** args) {
			if(argc == 1) {
				return new double(ceil(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloor(int argc, void** args) {
			if(argc == 1) {
				return new double(floor(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mCos(int argc, void** args) {
			if(argc == 1) {
				return new double(cos(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mSin(int argc, void** args) {
			if(argc == 1) {
				return new double(sin(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mTan(int argc, void** args) {
			if(argc == 1) {
				return new double(tan(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mPow(int argc, void** args) {
			if(argc == 2) {
				return new double(pow(*((double*)args[0]), *((double*)args[1])));
			}
			return new double(0.0);
		}

		void* mSqrt(int argc, void** args) {
			if(argc == 1) {
				return new double(sqrt(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mDegToRad(int argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * M_PI / 180.0);
			}
			return new double(0.0);
		}

		void* mRadToDeg(int argc, void** args) {
			if(argc == 1) {
				return new double(*((double*)args[0]) * 180.0 / M_PI);
			}
			return new double(0.0);
		}

		void* mLog(int argc, void** args) {
			if(argc == 1) {
				return new double(log(*((double*)args[0])));
			}
			return new double(0.0);
		}

		void* mFloatLength(int argc, void** args) {
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
				return new string(fmt::format(formatter, *((double*)args[0])));
			}
			return new string();
		}
	}
}