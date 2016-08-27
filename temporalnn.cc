#include "temporalnn.hh"
#include <algorithm>
#include <iostream>

namespace TemporalNet {
using namespace std;


/*
 * Time ops
 */
long long unsigned nano_to_milli(long long unsigned nano)
{
	return nano / pow(10, 6);
}

long long unsigned milli_to_nano(long long unsigned milli)
{
	return milli * pow(10, 6);
}

struct timespec ms_to_timespec(long long unsigned ltime)
{
	struct timespec ret = { 0 };

	ret.tv_sec = ltime / 1000;
	ret.tv_nsec = milli_to_nano(ltime % 1000);

	return ret;
}

long long unsigned timespec_to_ms(struct timespec time)
{
	long long ret = 0;

	ret += time.tv_sec * 1000;
	ret += nano_to_milli(time.tv_nsec);

	return ret;
}

struct timespec timespec_minus(struct timespec &time1, struct timespec &time2)
{
	long long ret;

	ret = time1.tv_sec - time2.tv_sec;
	ret *= 1000;
	ret += (nano_to_milli(time1.tv_nsec) - nano_to_milli(time2.tv_nsec));

	return ms_to_timespec(ret);
}

struct timespec timespec_plus(struct timespec &time1, struct timespec &time2)
{
	long long unsigned ret;

	ret = time1.tv_sec + time2.tv_sec;
	ret *= 1000;
	ret += (nano_to_milli(time1.tv_nsec) + nano_to_milli(time2.tv_nsec));

	return ms_to_timespec(ret);
}

long long timespec_plus(struct timespec &time, long long ms)
{
	return timespec_to_ms(time) + ms;
}

long long timespec_minus(struct timespec &time, long long ms)
{
	return timespec_to_ms(time) - ms;
}


/*
 * class Dendrite
 */
Dendrite::Dendrite(short delay, float seek, short cluster)
		: delaytime(delay), seekfactor(seek), clusterfactor(cluster),
		  bulge(0)
{
	clock_gettime(CLOCK_REALTIME, &firetime);
}

Neuron *Dendrite::setNeuron(Neuron *owner)
{
	neuron = owner;
	return neuron;
}

int Dendrite::input(short input_v)
{
	return neuron->input(input_v);
}

int Dendrite::grow(short input_v)
{
//  bulge += input_v * seekfactor;
	bulge += 1 * seekfactor;
	return neuron->handleDendriticBulge(bulge);
}



/*
 * class Axon
 */
Axon::Axon()
{
	n_output = vector<Neuron *>(0);
	d_output = vector<Dendrite *>(0);
}

Neuron *Axon::setNeuron(Neuron *owner)
{
	neuron = owner;
	return neuron;
}

int Axon::numberOfConnections()
{
	return d_output.size() + n_output.size();
}

void Axon::addDendriteOutput(Dendrite *d)
{
	for (auto it = d_output.begin(); it != d_output.end(); it++)
		if (*it == d)
			return;

	d_output.push_back(d);
}

void Axon::addNeuronOutput(Neuron *n)
{
	for (auto it = n_output.begin(); it != n_output.end(); it++)
		if (*it == n)
			return;

	n_output.push_back(n);
}

int Axon::fire(short input_v, struct timespec at_time)
{
	ulong n_dconnections = d_output.size();
	ulong n_nconnections = n_output.size();
	short dist_voltage = input_v / (n_dconnections ? n_dconnections : 1);

	if (n_dconnections == 0 && n_nconnections == 0)
		return -1;
	for (uint i = 0; i < n_dconnections; i++)
		d_output[i]->input(dist_voltage);
	/*
	 * Assume directly connected neurons indicate a "predetermined," strong correlation
	 * by exciting them with a full strength input (TODO: model neurotransmitter synapses.)
	 */
	for (uint i = 0; i < n_nconnections; i++)
		n_output[i]->input(input_v);

	return dist_voltage;
}



/*
 * class Neuron
 */
Neuron::Neuron()
{
	dendrites.push_back(Dendrite());
}

Neuron::Neuron(int xarg, int yarg)
{
	x = xarg;
	y = yarg;
	dendrites.push_back(Dendrite());
}

Neuron *Neuron::setNet(NeuralNet *owner)
{
	net = owner;
	return this;
}

Neuron *Neuron::setupDendrites()
{
	for (uint i = 0; i < dendrites.size(); i++)
		dendrites[i].setNeuron(this);
	return this;
}

Neuron *Neuron::setupAxon()
{
	return axon.setNeuron(this);
}

int Neuron::numberOfConnections()
{
	return axon.numberOfConnections();
}

int Neuron::input(short input_v, struct timespec at_time)
{
	long long time_delta = timespec_to_ms(timespec_minus(at_time, firetime));

	if (time_delta > excited_time)
		voltage = resting_v;
	if (time_delta <= refractory_time) {
		for (uint i = 0; i < dendrites.size(); i++)
			dendrites[i].grow(input_v);
		return -1;
	}

	voltage += input_v;
	if (voltage >= action_v) {
		firetime = at_time;
		return axon.fire(fire_v);
	}

	return 0;
}

int Neuron::fire()
{
	return input(abs(voltage - action_v));
}

void Neuron::addDendriteOutput(Neuron *n)
{
	axon.addDendriteOutput(&n->dendrites[0]);
}

void Neuron::addNeuronOutput(Neuron *n)
{
	axon.addNeuronOutput(n);
}

int Neuron::handleDendriticBulge(double bulge)
{
	return net->handleDendriticBulge(this, bulge);
}

int Neuron::input(short input_v)
{
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	return input(input_v, time);
}


/*
 * class NeuralNet
 */
NeuralNet::NeuralNet(int x, int y)
{
	neurons.resize(x, vector<Neuron>(y, Neuron()));
}

int NeuralNet::handleDendriticBulge(Neuron *n, double bulge)
{
	uint xpos = (uint)n->x;
	uint ypos = (uint)n->y;
	int scale = round(bulge);

	for (int i = 0; i <= scale; i++)
		for (int j = 0; j <= scale; j++)
			if (xpos + i < neurons.size() && ypos + j < neurons[xpos + i].size())
				neurons[xpos + i][ypos + j].addDendriteOutput(n);
	return n->numberOfConnections();
}

void NeuralNet::setupNeurons()
{
	for (uint i = 0; i < neurons.size(); i++)
		for (uint j = 0; j < neurons[i].size(); j++)
		{
			neurons[i][j].setNet(this);
			neurons[i][j].setupAxon();
			neurons[i][j].setupDendrites();
			neurons[i][j].x = i;
			neurons[i][j].y = j;
		}
}

void NeuralNet::connectTo(NeuralNet *net)
{
	for (uint i = 0; i < this->neurons.size(); i++)
		for (uint j = 0; j < this->neurons[i].size(); j++)
			if (net->neurons.size() < i && net->neurons[i].size() < j)
				this->neurons[i][j].addNeuronOutput(&net->neurons[i][j]);
	return;
}

} // namespace
