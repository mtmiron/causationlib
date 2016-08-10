#include <vector>
#include <map>
#include <math.h>
#include <time.h>


namespace TemporalNet {

using namespace std;


class Synapse;
class Dendrites;
class Neuron;
class Cluster;


class NeuralNet
{
  private:
	vector<Cluster> cluster;

  public:
	NeuralNet();
	~NeuralNet();

};


class Cluster : protected NeuralNet
{
  private:
	vector<Dendrites> dendrites;
};


class Neuron : protected NeuralNet
{
  private:
	char refractory_time = 50;
	char excited_time = 20;
	char refractory_v = -50;
	char resting_v = -80;
	char action_v = -30;
	char fire_v = 50;

  protected:
	vector<Synapse> synapse;
	vector<Dendrites> dendrites;

  public:
	Neuron(char reftime = 50, char excitetime = 20, char refv = -50,
			char rest_v = -80, char act_v = -30, char firev = 50)
		: refractory_time(reftime), excited_time(excitetime),
		  refractory_v(refv), resting_v(rest_v), action_v(act_v),
		  fire_v(firev)
	{ }

	~Neuron();
};


class Synapse : protected Neuron
{
  private:
	char vesicles;
	struct timespec time;

  public:
	Synapse(char vscls = 0) : vesicles(vscls) { }
	~Synapse();

	Neuron *output;
};


class Dendrites : protected Neuron
{
  private:
	char delaytime;
	char seekfactor;
	char clusterfactor;
	vector<Synapse> synapse;


  public:
	Dendrites(char delay, char seek, char cluster)
		: delaytime(delay), seekfactor(seek), clusterfactor(cluster)
	{ }

	~Dendrites();
};

} // namespace
