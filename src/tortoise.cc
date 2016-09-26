#include "tortoise.hh"
#include <iostream>

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
#ifdef BUILD_WITH_MULTITHREADING
	q_erase_mutex.lock();
#endif
	queue.insert( q_pair_t(time, func) );
#ifdef BUILD_WITH_MULTITHREADING
	q_erase_mutex.unlock();
#endif
	return;
}

void TimeQueue::applyAllUpto(const TortoiseTime time)
{
	if (queue.size() == 0)
		return;

#ifdef BUILD_WITH_MULTITHREADING
	q_erase_mutex.lock();
#endif
	for (auto next = queue.begin(); next != queue.end(); next = queue.begin())
	{
		if (next->first <= time)
			next->second();
		else
			break;
		queue.erase(next);
	}
#ifdef BUILD_WITH_MULTITHREADING
	q_erase_mutex.unlock();
#endif
}

int TimeQueue::nextIsEarlierThan(TortoiseTime &time)
{
	if (queue.size() == 0)
		return -1;

	auto next = queue.begin();
	return (TortoiseTime)next->first < time;
}

int TimeQueue::clear()
{
	int n;

	q_erase_mutex.lock();
	n = queue.size();
	queue.clear();
	q_erase_mutex.unlock();
	return n;
}

int TimeQueue::applyNext()
{
	if (queue.size() == 0)
		return -1;

	q_erase_mutex.lock();
	auto next = queue.begin();
	timed_call_t ret = next->second;
	queue.erase(next);
	q_erase_mutex.unlock();
	return ret();
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

TortoiseTime& TortoiseTime::operator=(TortoiseTime time)
{
	this->tv_sec = time.tv_sec;
	this->tv_nsec = time.tv_nsec;
	return *this;
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

bool TortoiseTime::operator<=(const TortoiseTime t2) const
{
	return ( (this->tv_sec < t2.tv_sec)
			|| ((this->tv_sec == t2.tv_sec)
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

bool TortoiseTime::operator==(const int i)
{
	return (this->tv_sec == i && this->tv_nsec == i);
}

bool TortoiseTime::operator==(const TortoiseTime t2)
{
	return (this->tv_sec == t2.tv_sec && this->tv_nsec == t2.tv_nsec);
}

TortoiseTime TortoiseTime::operator+(const TortoiseTime t2)
{
	struct timespec ret = { this->tv_sec + t2.tv_sec, 0 };

	ret.tv_sec += (this->tv_nsec + t2.tv_nsec) / pow(10,9);
	ret.tv_nsec = (this->tv_nsec + t2.tv_nsec) % (time_t)pow(10,9);

	return ret;
}

TortoiseTime TortoiseTime::operator-(const TortoiseTime t2)
{
	struct timespec ret = { this->tv_sec - t2.tv_sec, 0 };
	long long ns = this->tv_nsec - t2.tv_nsec;

	if (ns < 0) {
		ret.tv_sec += ns / pow(10,9);
		ret.tv_nsec = pow(10,9) + ns;
	} else {
		ret.tv_nsec = ns % (long)pow(10,9);
	}

	return ret;
}

TortoiseTime TortoiseTime::operator+(long long ms)
{
	struct timespec ret = { this->tv_sec, 0 };
	long long ns = this->tv_nsec + ms * pow(10,6);

	ret.tv_sec += ns / pow(10,9);
	ret.tv_nsec = ns % (long)pow(10,9);

	return ret;
}

TortoiseTime TortoiseTime::operator-(long long ms)
{
	struct timespec ret = { this->tv_sec, 0 };
	long long ns = this->tv_nsec - ms * pow(10,6);

	ret.tv_sec += ns / pow(10,9);
	if (ret.tv_sec == this->tv_sec)
		ret.tv_nsec = ns % (long)pow(10,9);
	else
		ret.tv_nsec = pow(10,9) - abs(ns);

	return ret;
}

ostream &operator<<(ostream &os, const TortoiseTime &t)
{
	char buf[256] = { 0 };
	TortoiseTime ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	sprintf(buf, "%10.010ld.%9.09ld (%10.010ld.%9.09lds ago)", t.tv_sec, t.tv_nsec, (ts - t).tv_sec, (ts - t).tv_nsec);
	return os << buf;
}
