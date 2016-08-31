#include "tortoise.hh"
#include <iostream>
#include <unistd.h>

using namespace std;

struct timespec ts1 = { 0, 0 };
struct timespec ts2 = { 0, 0 };
TortoiseTime t1, t2, t3;


void assert(bool x) {
	if (!x) cout << "t1: " << t1 << "\nt2: " << t2 << "\nt3: " << t3 << endl << endl;
}


void test_arithmetic() {
	t1 = (TortoiseTime)ts1;
	t2 = (TortoiseTime)ts2;
	t3 = t1 + t2;
//	assert(t3 - t2 == t1);
	/*
	t3 = t1 + t2;
	assert(t3 - t2 == t1);
	t3 = t2 - t1;
	assert(t3 + t1 == t2);
	t3 = t2 + t1;
	assert(t3 - t1 == t2);
	*/
	t2 = t3;
	t3 = t1 + ts2.tv_nsec / pow(10,6);
	assert(t1 + t3 == t1 + (t1 + t2));
}

int main()
{
	TortoiseTime t1, t2, t3;

	clock_gettime(CLOCK_REALTIME, &t1);
	clock_gettime(CLOCK_REALTIME, &t2);
	clock_gettime(CLOCK_REALTIME, &t3);

	cout << t1 << endl << t2 << endl << t3 << endl;
	cout << t2 - t1 << endl;
	cout << t3 - t1 << endl;
	cout << t3 - t2 - t1 << endl;
	return 0;
	cout << t1 << endl << endl;
	cout << t1 + 5 << endl;
	cout << t1 + 50 << endl;
	cout << t1 + 150 << endl << endl;
	cout << t1 - 5 << endl;
	cout << t1 - 50 << endl;
	cout << t1 - 150 << endl;
	return 0;

	for (int i = 0; i < 1000; i++)
	{
		clock_gettime(CLOCK_REALTIME, &ts1);
		srandom(ts1.tv_nsec);
		int r = random() % 100;
		t1 = ts1;
		usleep(ts1.tv_nsec);
		clock_gettime(CLOCK_REALTIME, &ts1);
		t2 = ts1;
		t3 = t2 - t1;
		assert(t3.tv_sec == t2.tv_sec - t1.tv_sec);
		assert(t3.tv_nsec == t2.tv_nsec - t1.tv_nsec);
		ts2 = { ts1.tv_sec, ts1.tv_nsec + r };
		test_arithmetic();
		ts2 = { ts1.tv_sec + 1, ts1.tv_nsec - r };
		test_arithmetic();
		ts2 = { 0, r };
		test_arithmetic();
	}

	return 0;
}
