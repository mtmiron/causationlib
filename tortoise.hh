#include <time.h>

namespace TemporalNet {

struct TortoiseTime : timespec
{
	TortoiseTime();
	TortoiseTime(const struct timespec &t);
	TortoiseTime operator-(long long ms);
	TortoiseTime operator+(long long ms);
	TortoiseTime operator+(const TortoiseTime &t2);
	TortoiseTime operator-(const TortoiseTime &t2);
};

} // namespace
