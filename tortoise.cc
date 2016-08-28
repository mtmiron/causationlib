#include "tortoise.hh"

/*
 * Slow and steady... :)
 */
using namespace TemporalNet;
using namespace std;


void TimeQueue::insert(struct TortoiseTime time, timed_call_t func)
{
	lock_guard<mutex> lock(q_mutex);

	queue.insert( q_pair_t(time, func) );
	return;
}

int TimeQueue::applyNext()
{
	int ret;
	auto elem = [=]() {
		lock_guard<mutex> lock(q_mutex);
		auto elem = queue.begin();
		//elem->second(elem->first);
		return elem;
	};
	return 0;
}

/*
 * class TortoiseTime
 */
TortoiseTime::TortoiseTime()
{
	tv_sec = 0;
	tv_nsec = 0;
}

TortoiseTime::TortoiseTime(const struct timespec &t)
{
	this->tv_sec = t.tv_sec;
	this->tv_nsec = t.tv_nsec;
}

TortoiseTime::TortoiseTime(const struct TortoiseTime &t)
{
	this->tv_sec = t.tv_sec;
	this->tv_nsec = t.tv_nsec;
}

TortoiseTime::TortoiseTime (int init)
{
	tv_sec = init;
	tv_nsec = init;
}

bool TortoiseTime::operator() (const struct TortoiseTime &t1, const struct TortoiseTime &t2)
{
}

bool TortoiseTime::operator< (const TortoiseTime &t1)//, const TortoiseTime &t2)
{
	TortoiseTime t2 = *this;
	if (t1.tv_sec < t2.tv_sec)
		return true;
	else if (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec)
		return true;
	else
		return false;
}

bool TortoiseTime::operator>(const TortoiseTime &t2)
{
	if (this->tv_sec > t2.tv_sec)
		return true;
	else if (this->tv_sec == t2.tv_sec && this->tv_nsec > t2.tv_nsec)
		return true;
	else
		return false;
}

bool TortoiseTime::operator>(unsigned short ms)
{
	if (this->tv_nsec > ms * pow(10,6))
		return true;
	else
		return false;
}

bool TortoiseTime::operator<=(const TortoiseTime &t2)
{
	if (this->tv_sec < t2.tv_sec)
		return true;
	else if (this->tv_sec == t2.tv_sec && this->tv_nsec <= t2.tv_nsec)
		return true;
	else
		return false;
}

bool TortoiseTime::operator<=(unsigned short ms)
{
	if (this->tv_nsec <= (ms * pow(10,6)))
		return true;
	else
		return false;
}

bool TortoiseTime::operator>=(const TortoiseTime &t2)
{
	if (this->tv_sec > t2.tv_sec)
		return true;
	else if (this->tv_sec == t2.tv_sec && this->tv_nsec >= t2.tv_nsec)
		return true;
	else
		return false;
}

bool TortoiseTime::operator==(const TortoiseTime &t2)
{
	if (this->tv_sec == t2.tv_sec && this->tv_nsec == t2.tv_nsec)
		return true;
	else
		return false;
}

TortoiseTime TortoiseTime::operator+(const TortoiseTime &t2)
{
	struct timespec ret;

	ret.tv_sec = this->tv_sec + t2.tv_sec;
	ret.tv_nsec = this->tv_nsec + t2.tv_nsec;

	return ret;
}

TortoiseTime TortoiseTime::operator-(const TortoiseTime &t2)
{
	struct timespec ret;

	ret.tv_sec = this->tv_sec - t2.tv_sec;
	ret.tv_nsec = this->tv_nsec - t2.tv_nsec;
	return ret;
}

TortoiseTime TortoiseTime::operator+(long long ms)
{
	struct timespec ret;

	ms *= pow(10,6);
	ret.tv_sec = this->tv_sec + (ms / pow(10,9));
	ret.tv_nsec = this->tv_nsec + ms;

	return ret;
}

TortoiseTime TortoiseTime::operator-(long long ms)
{
	struct timespec ret;

	ret.tv_sec = this->tv_sec;
	ret.tv_nsec = this->tv_nsec - ms * pow(10,6);

	return ret;
}


/*
 * TODO: refactor these out
 */
long long unsigned nano_to_milli(long long nano)
{
//	return nano / pow(10, 6);
}

long long unsigned milli_to_nano(long long milli)
{
//	return milli * pow(10, 6);
}

struct timespec ms_to_timespec(long long ltime)
{
	/*
	struct timespec ret = { 0 };

	ret.tv_sec = ltime / 1000;
	ret.tv_nsec = milli_to_nano(ltime % 1000);

	return ret;
	*/
}

long long unsigned timespec_to_ms(struct timespec time)
{
	/*
	long long ret = 0;

	ret += time.tv_sec * 1000;
	ret += nano_to_milli(time.tv_nsec);

	return ret;
	*/
}

struct timespec timespec_minus(struct timespec &time1, struct timespec &time2)
{
	/*
	long long ret;

	ret = time1.tv_sec - time2.tv_sec;
	ret *= 1000;
	ret += nano_to_milli(time1.tv_nsec - time2.tv_nsec);

	return ms_to_timespec(ret);
	*/
}

struct timespec timespec_plus(struct timespec &time1, struct timespec &time2)
{
	/*
	long long unsigned ret;

	ret = time1.tv_sec + time2.tv_sec;
	ret *= 1000;
	ret += (nano_to_milli(time1.tv_nsec) + nano_to_milli(time2.tv_nsec));

	return ms_to_timespec(ret);
	*/
}

long long timespec_plus(struct timespec &time, long long ms)
{
//	return timespec_to_ms(time) + ms;
}

long long timespec_minus(struct timespec &time, long long ms)
{
//	return timespec_to_ms(time) - ms;
}
