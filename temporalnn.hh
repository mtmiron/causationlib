#ifndef _TEMPORALNN_HH_INCLUDED
#define _TEMPORALNN_HH_INCLUDED 1

#include <math.h>
#include <time.h>
#include <vector>
#include <unordered_set>
#include <opencv2/core.hpp>
#include "tortoise.hh"

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


class BrainCell
{
  protected:
	struct TortoiseTime firetime;
	unsigned short propagation_time = 5;

  public:
	Neuron *neuron;
	NeuralNet *net;

	explicit BrainCell(Neuron *n);
	Neuron *setNeuron(Neuron *owner);
	NeuralNet *setNet(NeuralNet *owner);
	virtual int input(short input_v, struct TortoiseTime &at_time) = 0;
};


class Dendrite : public BrainCell
{
  friend class Neuron;
  friend class Axon;
  private:
	short delaytime = 0;
	float seekfactor = 0.1;
	short clusterfactor = 1;
	float bulge = 0;
	int input(short input_v, struct TortoiseTime &at_time);
	int grow();

  public:
	explicit Dendrite(Neuron *n);
};


class Axon : public BrainCell
{
  friend class Neuron;
  private:
	short vesicles = 0;

	unordered_set<Dendrite *> d_output;
	unordered_set<Neuron *> n_output;

  public:
	explicit Axon(Neuron *n);
	void addDendriteOutput(Dendrite *d);
	void addNeuronOutput(Neuron *n);
	int numberOfConnections();
	int input(short input_v, struct TortoiseTime &at_time);
};


class Neuron : public BrainCell
{
  friend class NeuralNet;
  private:
  	// In milliseconds
	unsigned short refractory_time = 50;
	unsigned short excited_time = 20;
	short refractory_v = -50;
	short voltage = -80;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;
	int x = 0;
	int y = 0;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;

	void addNeuronOutput(Neuron *n);
	void addDendriteOutput(Neuron *n);

  public:
	explicit Neuron();
	Neuron *setupDendrites();
	Neuron *setupAxon();
	int numberOfConnections();
	int input(short input_v, struct TortoiseTime &at_time);
	int fire();
};

class NeuralNet
{
  public:
	vector< vector<Neuron> > neurons;

	explicit NeuralNet(int x = 100, int y = 100);
	void connectTo(NeuralNet *net);
	int handleDendriticBulge(Neuron *n, float bulge);
	cv::Mat createConnectionDensityImage(int height, int width);
	cv::Mat createCurrentActivityImage(int height, int width, struct TortoiseTime &at_time);
};

} // namespace

#endif
