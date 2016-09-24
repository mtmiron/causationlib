#include "../src/tortoise.hh"
#include <iostream>
#include <unistd.h>

using namespace std;

struct timespec ts1 = { 0, 0 };
struct timespec ts2 = { 0, 0 };
TortoiseTime t1, t2, t3;


int main()
{
	TortoiseTime t1, t2, t3;

	clock_gettime(CLOCK_REALTIME, &t1);
	clock_gettime(CLOCK_REALTIME, &t2);
	clock_gettime(CLOCK_REALTIME, &t3);

	cout << t1 << "\tt1" << endl << t2 << "\tt2" << endl << t3 << "\tt3" << endl << endl;
	cout << t2 - t1 << "\tt2 - t1" << endl;
	cout << t3 - t1 << "\tt3 - t1" << endl;
	cout << t3 - t2 - t1 << "\tt3 - t2 - t1" << endl << endl;

	cout << t1 + 5 << "\tt1 + 5ms" << endl;
	cout << t1 + 50 << "\tt1 + 50ms" << endl;
	cout << t1 + 150 << "\tt1 + 150ms" << endl << endl;
	cout << t1 - 5 << "\tt1 - 5ms" << endl;
	cout << t1 - 50 << "\tt1 - 50ms" << endl;
	cout << t1 - 150 << "\tt1 - 150ms" << endl << endl;

	cout << "t1 <= t1: " << (t1 <= t1) << endl;
	cout << "t1 <= t2: " << (t1 <= t2) << endl;
	cout << "t1 >= t2: " << (t1 >= t2) << endl;
	cout << "t1 == t1: " << (t1 == t1) << endl;
	cout << "t1 > t2:  " << (t1 > t2) << endl;
	cout << "t1 < t2:  " << (t1 < t2) << endl;
	cout << "t1 < t1:  " << (t1 < t1) << endl;
}
