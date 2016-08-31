#ifndef _TORTOISE_HH_INCLUDED
#define _TORTOISE_HH_INCLUDED 1

#include <time.h>
#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <math.h>

using namespace std;

static mutex q_insert_mutex;
static mutex q_apply_mutex;


struct TortoiseTime : timespec
{
	TortoiseTime();
	TortoiseTime(int init);
	TortoiseTime(const struct timespec &t);
	TortoiseTime(const struct TortoiseTime &t);

	TortoiseTime& operator=(TortoiseTime time);

	TortoiseTime operator-(long long ms);
	TortoiseTime operator+(long long ms);
	TortoiseTime operator+(TortoiseTime t2);
	TortoiseTime operator-(TortoiseTime t2);

	bool operator<= (unsigned short ms);
	bool operator> (unsigned short ms);
	bool operator> (const TortoiseTime t2);
	bool operator< (const TortoiseTime t2) const;
	bool operator<= (const TortoiseTime t2) const;
	bool operator>= (const TortoiseTime t2);
	bool operator== (const TortoiseTime t2);
	bool operator== (const int i);
	friend ostream& operator<<(ostream& os, const TortoiseTime &t);
};

typedef function<int()> timed_call_t;
typedef pair<TortoiseTime, timed_call_t> q_pair_t;

class TimeQueue
{
  private:
	multimap< TortoiseTime, timed_call_t > queue;

  public:
	void insert(TortoiseTime, timed_call_t);
	void applyAllBefore(const TortoiseTime time);
	int nextIsEarlierThan(TortoiseTime &time);
	int applyNext();
	int size();
};

#endif
