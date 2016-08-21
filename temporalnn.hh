#include <vector>
#include <math.h>
#include <time.h>


namespace TemporalNet {
using namespace std;

long nano_to_milli(long nano);
long milli_to_nano(long milli);


class Neuron;
class NeuralNet;

class Dendrite
{
  private:
	short delaytime = 0;
	float seekfactor = 0;
	short clusterfactor = 0;
	unsigned int bulge = 0;
	struct timespec time = { 0 };
	Neuron *neuron = NULL;

  public:
	Dendrite(short delay = 0, float seek = 0.1, short cluster = 1);
	Neuron *setNeuron(Neuron *owner);

	int fire(short input_v);
	int grow(short input_v);
};


class Axon
{
  private:
	short vesicles = 0;
	struct timespec time = { 0 };
	Neuron *neuron = NULL;

	vector<Dendrite *> d_output;
	vector<Neuron *> n_output;

  public:
	Axon();
	Neuron *setNeuron(Neuron *owner);
	void addDendriteOutput(Dendrite *d);
	int numberOfConnections();
	int fire(short input_v);
};


class Neuron
{
  private:
	short refractory_time = 2;
	short excited_time = 20;
	short refractory_v = -50;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;
	struct timespec time = { 0 };
	NeuralNet *net = NULL;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;

  public:
	Neuron(short reftime = 50, short excitetime = 20, short refv = -50,
			short rest_v = -80, short act_v = -30, short firev = 50);
	Neuron(int xarg, int yarg);
	Neuron *setNet(NeuralNet *owner);
	Neuron *setupDendrites();
	Neuron *setupAxon();
	void addConnection(Neuron *n);
	int handleDendriticBulge(float bulge);
	int numberOfConnections();
	int fire(short input_v);
	int x = 0;
	int y = 0;
};

class NeuralNet
{
  public:
	vector< vector<Neuron> > neurons;

	NeuralNet(int x = 100, int y = 100);
	int handleDendriticBulge(Neuron *n, float bulge);
	void setupNeurons();
};

} // namespace
