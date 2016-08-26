#include <vector>
#include <math.h>
#include <time.h>
#include <opencv2/core.hpp>


/*
 * A big, big caveat with the simplicity of this implementation: events do not
 * actually take place at the specified time provided by an argument -- there is no
 * dispatch queue, signal timers, or anything of the sort.  The time value is only
 * used to calculate summed voltage differences.
 */
namespace TemporalNet {
using namespace std;

class Neuron;
class NeuralNet;


long long unsigned nano_to_milli(long long unsigned nano);
long long unsigned milli_to_nano(long long unsigned milli);
long long unsigned timespec_to_ms(struct timespec time);
struct timespec ms_to_timespec(long long unsigned ltime);
long long timespec_minus(struct timespec &time, long long ms);
struct timespec timespec_minus(struct timespec &time1, struct timespec &time2);
long long timespec_plus(struct timespec &time, long long ms);
struct timespec timespec_plus(struct timespec &time1, struct timespec &time2);


class Dendrite
{
  friend class Neuron;
  friend class Axon;
  private:
	short delaytime = 0;
	double seekfactor = 0;
	short clusterfactor = 0;
	double bulge = 0;
	struct timespec firetime = { 0 };
	int input(short input_v);
	int grow(short input_v);
	Neuron *neuron = NULL;

  public:
	Dendrite(short delay = 0, float seek = 0.1, short cluster = 1);
	Neuron *setNeuron(Neuron *owner);
};


class Axon
{
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
	void addNeuronOutput(Neuron *n);
	int numberOfConnections();
	int fire(short input_v, struct timespec at_time = { 0 });
};


class Neuron
{
  friend class NeuralNet;
  friend class Dendrite;
  friend class Axon;
  private:
  	// In milliseconds
	unsigned short refractory_time = 50;
	unsigned short excited_time = 20;
	short refractory_v = -50;
	short voltage = -80;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;
	short propagation_time = 5;
	int x = 0;
	int y = 0;
	struct timespec firetime = { 0 };
	NeuralNet *net = NULL;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;

	void addNeuronOutput(Neuron *n);
	void addDendriteOutput(Neuron *n);

  public:
	Neuron();
	Neuron(int xarg, int yarg);
	Neuron *setNet(NeuralNet *owner);
	Neuron *setupDendrites();
	Neuron *setupAxon();
	int handleDendriticBulge(double bulge);
	int numberOfConnections();
	int input(short input_v = 1);
	int input(short input_v, struct timespec at_time);
	int fire();
};

class NeuralNet
{
  public:
	vector< vector<Neuron> > neurons;

	NeuralNet(int x = 100, int y = 100);
	int handleDendriticBulge(Neuron *n, double bulge);
	void setupNeurons();
	cv::Mat createConnectionDensityImage(int height, int width);
	cv::Mat createCurrentActivityImage(int height, int width, struct timespec at_time = { 0 });
	void connectTo(NeuralNet *net);
};

} // namespace
