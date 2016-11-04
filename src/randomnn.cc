#include "randomnn.hh"

#define MAX_PROPAGATION_TIME 20
#define MAX_REFRACTORY_TIME 50
#define MAX_EXCITED_TIME 100
#define MAX_DENDRITE_BULGE 5
#define MAX_ACTION_VOLTAGE 100
#define MAX_FIRE_VOLTAGE 100
#define MAX_RESTING_VOLTAGE 100

RandomNet::RandomNet(int x, int y) : NeuralNet(x, y)
{
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	srandom(ts.tv_sec);
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			neurons[i][j].setPropagationTime(random() % MAX_PROPAGATION_TIME);
			neurons[i][j].setRefractoryTime(random() % MAX_REFRACTORY_TIME);
			neurons[i][j].setExcitedTime(random() % MAX_EXCITED_TIME);
			neurons[i][j].action_v = 0 - (random() % MAX_ACTION_VOLTAGE);
			neurons[i][j].fire_v = 0 - (random() % MAX_FIRE_VOLTAGE);
			neurons[i][j].resting_v = 0 - (random() % MAX_RESTING_VOLTAGE);
			neurons[i][j].addDendriteOutput(&neurons[random() % x][random() % y]);
		}
}
