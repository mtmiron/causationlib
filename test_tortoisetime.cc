#include "tortoise.hh"
#include <iostream>
#include <unistd.h>

using namespace std;

struct timespec ts1 = { 0, 0 };
struct timespec ts2 = { 0, 0 };
TortoiseTime t1, t2, t3;


void assert(bool x) {
	if (!x) cout << "t1: " << t1 << "t2: " << t2 << "t3: " << t3 << endl;
}


void test_arithmetic() {
	t1 = (TortoiseTime)ts1;
	t2 = (TortoiseTime)ts2;
	t3 = t1 - t2;
	assert(t3 + t2 == t1);
	t3 = t1 + t2;
	assert(t3 - t2 == t1);
	t3 = t2 - t1;
	assert(t3 + t1 == t2);
	t3 = t2 + t1;
	assert(t3 - t1 == t2);
}

int main()
{


	for (int i = 0; i < 1000; i++)
	{
		clock_gettime(CLOCK_REALTIME, &ts1);
		int r = rand() % 10000000;
		ts2 = { ts1.tv_sec + 1, ts2.tv_nsec + r };
		test_arithmetic();
		ts2 = { ts1.tv_sec + 1, ts2.tv_nsec - r };
	}

	return 0;
}
