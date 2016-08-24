#include <vector>
#include <math.h>
#include <time.h>
#include <opencv2/core.hpp>


namespace TemporalNet {
using namespace std;

class Neuron;
class NeuralNet;


long long nano_to_milli(long long nano);
long long milli_to_nano(long long milli);
long long timespec_minus(struct timespec &time1, struct timespec &time2);


class Dendrite
{
  friend class NeuralNet;
  private:
	short delaytime = 0;
	double seekfactor = 0;
	short clusterfactor = 0;
	double bulge = 0;
	struct timespec firetime = { 0 };
	Neuron *neuron = NULL;

  public:
	Dendrite(short delay = 0, float seek = 0.1, short cluster = 1);
	Neuron *setNeuron(Neuron *owner);

	int fire(short input_v);
	int grow(short input_v);
};


class Axon
{
  friend class NeuralNet;
  private:
	short vesicles = 0;
	struct timespec firetime = { 0 };
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
  friend class NeuralNet;
  friend class Dendrite;
  friend class Axon;
  private:
  	// In milliseconds
	short refractory_time = 20;
	short excited_time = 5;
	short refractory_v = -50;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;
	int x = 0;
	int y = 0;
	struct timespec firetime = { 0 };
	NeuralNet *net = NULL;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;
	int fire(short input_v = 50);

  public:
	Neuron(short reftime = 50, short excitetime = 20, short refv = -50,
			short rest_v = -80, short act_v = -30, short firev = 50);
	Neuron(int xarg, int yarg);
	Neuron *setNet(NeuralNet *owner);
	Neuron *setupDendrites();
	Neuron *setupAxon();
	void addConnection(Neuron *n);
	int handleDendriticBulge(double bulge);
	int numberOfConnections();
	int input(short input_v = 1);
};

class NeuralNet
{
  public:
	vector< vector<Neuron> > neurons;

	NeuralNet(int x = 100, int y = 100);
	int handleDendriticBulge(Neuron *n, double bulge);
	void setupNeurons();
	cv::Mat createConnectionDensityImage(int height, int width);
	cv::Mat createCurrentActivityImage(int height, int width);
};

} // namespace
