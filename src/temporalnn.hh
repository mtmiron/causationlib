#ifndef TEMPORALNN_HH_INCLUDED
#define TEMPORALNN_HH_INCLUDED 1

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
	TortoiseTime input_time;
	unsigned short propagation_time = 1;
	unsigned int max_dendrite_bulge = 50;

  public:
	TortoiseTime firetime;
	static bool freeze_connections;
	static TimeQueue event_queue;
	Neuron *neuron;
	NeuralNet *net;

	explicit BrainCell(Neuron *n);
	Neuron *setNeuron(Neuron *owner);
	NeuralNet *setNet(NeuralNet *owner);
	virtual int input(short input_v, TortoiseTime at_time) = 0;
#ifdef BUILD_WITH_TIMEQUEUE
	virtual int bound_input(short input_v, TortoiseTime at_time) = 0;
#endif
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
	int input(short input_v, TortoiseTime at_time);
#ifdef BUILD_WITH_TIMEQUEUE
	int bound_input(short input_v, TortoiseTime at_time);
#endif
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
	int input(short input_v, TortoiseTime at_time);
#ifdef BUILD_WITH_TIMEQUEUE
	int bound_input(short input_v, TortoiseTime at_time);
#endif
};


class Neuron : public BrainCell
{
  friend class NeuralNet;
  private:
  	// In milliseconds
	unsigned short refractory_time = 50;
	unsigned short excited_time = 10;
	short refractory_v = -50;
	short voltage = -80;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;

  protected:
	int x = 0;
	int y = 0;
	Axon axon;
	vector<Dendrite> dendrites;

	void addNeuronOutput(Neuron *n);
	void addDendriteOutput(Neuron *n);

  public:
	Neuron();
	Neuron *setupDendrites();
	Neuron *setupAxon();
	int numberOfConnections();
	int input(short input_v, TortoiseTime at_time);
#ifdef BUILD_WITH_TIMEQUEUE
	int bound_input(short input_v, TortoiseTime at_time);
#endif
	int fire();
	void setPropagationTime(int prop);
	void setMaxDendriteConnections(unsigned int max);
	friend ostream &operator<<(ostream &os, Neuron &neuron);
};

class NeuralNetException : exception
{
	virtual const char *what() const throw()
	{
		return "bad parameters";
	}
};

class NeuralNet
{
  protected:
	int dim_x = 0;
	int dim_y = 0;
	int dim_z = 0;

  public:
	vector< vector<Neuron> > neurons;

	NeuralNet(int x = 100, int y = 100);
	virtual void connectTo(NeuralNet *net);
	virtual int handleDendriticBulge(Neuron *n, float bulge);
	virtual cv::Mat createConnectionDensityImage(int width, int height);
	virtual cv::Mat createCurrentActivityImage(int width, int height, TortoiseTime at_time, int fade_time = 1);
	virtual cv::Mat &createInputActivityImage(cv::Mat &image, TortoiseTime at_time, int fade_time = 1);
	virtual Neuron &getFromWindowPosition(int Px, int Py, int Wx, int Wy);
};


#endif
