#ifndef TORTOISE_HH_INCLUDED
#define TORTOISE_HH_INCLUDED 1

#include <time.h>
#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <math.h>

using namespace std;

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
	recursive_mutex q_insert_mutex;
	recursive_mutex q_apply_mutex;
	recursive_mutex q_erase_mutex;


  public:
	void insert(TortoiseTime, timed_call_t);
	void applyAllUpto(const TortoiseTime time);
	int nextIsEarlierThan(TortoiseTime &time);
	int applyNext();
	int size();
	int clear();
};

#endif
