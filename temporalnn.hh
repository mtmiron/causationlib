#include <vector>
#include <map>
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
	short delaytime;
	short seekfactor;
	short clusterfactor;
	unsigned int bulge;
	struct timespec time;
	Neuron *neuron;

  public:
	Dendrite(short delay = 0, short seek = 1, short cluster = 1);
	Dendrite *setNeuron(Neuron *owner);

	int fire(short input_v);
	int grow(short input_v);
};


class Axon
{
  private:
	short vesicles;
	struct timespec time;

	vector<Dendrite *> d_output;
	vector<Neuron *> n_output;

  public:
	Axon(short vscls = 0);

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
	struct timespec time;
	NeuralNet *net;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;

  public:
	Neuron(short reftime = 50, short excitetime = 20, short refv = -50,
			short rest_v = -80, short act_v = -30, short firev = 50);

	Neuron *setNet(NeuralNet *owner);
	Neuron *setupDendrites();
	int fire(short input_v);
};

class NeuralNet
{
  public:
	vector< vector<Neuron> > neurons;

	NeuralNet(int x = 100, int y = 100);
	void setupNeurons();
};

} // namespace
