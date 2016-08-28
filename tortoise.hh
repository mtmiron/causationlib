#ifndef _TORTOISE_HH_INCLUDED
#define _TORTOISE_HH_INCLUDED 1

#include <time.h>
#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <math.h>

namespace TemporalNet {
using namespace std;

static mutex q_mutex;

typedef function<int(short,struct TortoiseTime)> timed_call_t;
typedef pair<struct TortoiseTime, timed_call_t> q_pair_t;

struct TortoiseTime : timespec
{
	TortoiseTime();
	TortoiseTime(int init);
	TortoiseTime(const struct timespec &t);
	TortoiseTime(const struct TortoiseTime &t);

	TortoiseTime operator-(long long ms);
	TortoiseTime operator+(long long ms);
	TortoiseTime operator+(const TortoiseTime &t2);
	TortoiseTime operator-(const TortoiseTime &t2);

	bool operator<= (unsigned short ms);
	bool operator() (const struct TortoiseTime &t1, const struct TortoiseTime &t2);
	bool operator> (const TortoiseTime &t2);
	bool operator> (unsigned short ms);
//	bool operator< (const struct TortoiseTime &t1, const struct TortoiseTime &t2);
	bool operator< (const TortoiseTime &t2);
	bool operator<= (const TortoiseTime &t2);
	bool operator>= (const TortoiseTime &t2);
	bool operator== (const TortoiseTime &t2);
};

class TimeQueue
{
  private:
	multimap< struct TortoiseTime, timed_call_t, TortoiseTime > queue;

  public:
	void insert(struct TortoiseTime, timed_call_t);
	int applyNext();
};

}
#endif
