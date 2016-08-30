#include "tortoise.hh"

/*
 * Slow and steady... :)
 */
using namespace std;

/*
 * class TimeQueue
 */
int TimeQueue::size()
{
	return queue.size();
}

void TimeQueue::insert(TortoiseTime time, timed_call_t func)
{
	lock_guard<mutex> lock(q_insert_mutex);

	queue.insert( q_pair_t(time, func) );
	return;
}

int TimeQueue::applyNext()
{
	if (queue.size() == 0)
		return -1;

	lock_guard<mutex> lock(q_apply_mutex);
	auto next = queue.begin();
	int ret = next->second();
	queue.erase(next);
	return ret;
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

bool TortoiseTime::operator< (const TortoiseTime t2) const
{
	return ( (this->tv_sec < t2.tv_sec)
			|| ( (this->tv_sec == t2.tv_sec)
				&& (this->tv_nsec < t2.tv_nsec)) );
}

bool TortoiseTime::operator>(const TortoiseTime t2)
{
	return ( (this->tv_sec > t2.tv_sec)
			|| ( (this->tv_sec == t2.tv_sec)
				&& (this->tv_nsec > t2.tv_nsec)) );
}

bool TortoiseTime::operator>(unsigned short ms)
{
	return (this->tv_nsec > ms * pow(10,6));
}

bool TortoiseTime::operator<=(const TortoiseTime t2)
{
	return ( (this->tv_sec < t2.tv_sec)
			|| ( (this->tv_sec == t2.tv_sec)
				&& (this->tv_nsec <= t2.tv_nsec)) );
}

bool TortoiseTime::operator<=(unsigned short ms)
{
	return (this->tv_nsec <= (ms * pow(10,6)));
}

bool TortoiseTime::operator>=(const TortoiseTime t2)
{
	return ( (this->tv_sec > t2.tv_sec)
			|| ( (this->tv_sec == t2.tv_sec)
				&& (this->tv_nsec >= t2.tv_nsec)) );
}

bool TortoiseTime::operator==(const TortoiseTime t2)
{
	return (this->tv_sec == t2.tv_sec && this->tv_nsec == t2.tv_nsec);
}

TortoiseTime TortoiseTime::operator+(const TortoiseTime t2)
{
	struct timespec ret;

	ret.tv_sec = this->tv_sec + t2.tv_sec;
	ret.tv_nsec = this->tv_nsec + t2.tv_nsec;

	return ret;
}

TortoiseTime TortoiseTime::operator-(const TortoiseTime t2)
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
