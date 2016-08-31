#include "../tortoise.hh"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;


int func(TortoiseTime t)
{
		std::cout << t << endl;
		return 0;
}

int main()
{
	TimeQueue queue;
	TortoiseTime time;

	clock_gettime(CLOCK_REALTIME, &time);

	for (int i = 0; i < 10; i++)
	{
		time = time + (random() % 10000);
		queue.insert(time, std::bind(func, time));
	}

	clock_gettime(CLOCK_REALTIME, &time);
	time.tv_sec += 10;
	cout << "Applying all before " << time << "..." << endl;
	queue.applyAllBefore(time);
	cout << endl << "Applying the rest..." << endl;
	while (queue.applyNext() != -1)
		;
	cout << endl;
	
	return 0;
}
