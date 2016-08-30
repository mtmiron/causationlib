#ifndef _TEMPORALNN_HH_INCLUDED
#define _TEMPORALNN_HH_INCLUDED 1

#include <math.h>
#include <time.h>
#include <vector>
#include <unordered_set>
#include <opencv2/core.hpp>
#include "tortoise.hh"


using namespace std;

class Neuron;
class NeuralNet;


class BrainCell
{
  protected:
	TortoiseTime firetime;
	unsigned short propagation_time = 5;

  public:
	static TimeQueue event_queue;
	Neuron *neuron;
	NeuralNet *net;

	explicit BrainCell(Neuron *n);
	Neuron *setNeuron(Neuron *owner);
	NeuralNet *setNet(NeuralNet *owner);
	virtual int input(short input_v, struct TortoiseTime &at_time) = 0;
	virtual int bound_input(short input_v, struct TortoiseTime &at_time) = 0;
};

// without this, the compiler leaves the symbol undefined in the object file
// -- don't ask me, but (compiler == happy) == (programmer == happy)
TimeQueue BrainCell::event_queue;

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
	int bound_input(short input_v, struct TortoiseTime &at_time);
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
	int bound_input(short input_v, struct TortoiseTime &at_time);
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
	int bound_input(short input_v, struct TortoiseTime &at_time);
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


#endif
