#include "tortoise.hh"
#include <iostream>

/*
 * Slow and steady... :)
 */
using namespace std;

/*
 * class TimeQueue
 *
 * The TimeQueue class is a quick, easy way of ensuring that the temporal
 * component of the neural net is preserved perfectly.
 *
 * A less computationally expensive method would be to divide events
 * up into time slices, and use standard numerical methods to minimize
 * the amount of error per time slice (other neurophysiology simulations
 * tend to do this, allowing the user to define the acceptable error.)
 *
 * The basis of the temporalnn.cc design is that every event defines the
 * amount of time that it takes until completion (completes at T + N, where T is the
 * start time and N is the duration of the event.)  To implement this,
 * the TimeQueue class maintains a queue that's automatically sorted
 * by time (via a Multimap object.)  If compiling with the TimeQueue is
 * enabled (the default), all neural net events are bound with a STL
 * closure and inserted into the TimeQueue object for execution in temporal
 * order.
 *
 * The disadvantage of this method is that it essentially reduces the speed
 * of execution to that of an interpreted language (all function calls
 * within the neural net are "abstracted" closures), but without it,
 * the sequence of events in time would be an uncontrollable function of
 * the order of neuron stimulation.
 *
 * To illustrate, if a neuron fires at one end of a net layer, and a
 * neuron at the other end of a net layer fires simultaneously, even
 * with both events executing on independent CPU cores, the resulting
 * spread of stimulation triggered by the events would be unpredictable: they
 * have to combine properly at every single point in the neural net, regardless
 * of how long the "chain" of stimulation might be or what its unique
 * properties are.
 *
 * Since every neuron has adjustable parameters for the amount of time
 * it takes the signal fired off from it to hit the next neuron, as well
 * as the voltage change it causes (which allows for neurons to inhibit
 * other neurons just like in human neurophysiology), an approximation
 * would make the results of the simulation unreliable due to dependence on
 * uncontrollable factors.
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
