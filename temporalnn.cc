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
 * class BrainCell
 */
BrainCell::BrainCell(Neuron *n)
{
	neuron = n;
}

Neuron *BrainCell::setNeuron(Neuron *owner)
{
	neuron = owner;
	return neuron;
}

NeuralNet *BrainCell::setNet(NeuralNet *owner)
{
	net = owner;
	return net;
}


/*
 * class Dendrite
 */
Dendrite::Dendrite(Neuron *n) : BrainCell(n)
{
	this->neuron = n;
}

int Dendrite::input(short input_v, struct timespec &at_time)
{
	return neuron->input(input_v, at_time);
}

int Dendrite::grow()
{
//  bulge += input_v * seekfactor;
	bulge += 1 * seekfactor;
	return this->neuron->net->handleDendriticBulge(this->neuron, bulge);
}



/*
 * class Axon
 */
Axon::Axon(Neuron *n) : BrainCell(n)
{
}

int Axon::numberOfConnections()
{
	return d_output.size() + n_output.size();
}

void Axon::addDendriteOutput(Dendrite *d)
{
	d_output.insert(d);
}

void Axon::addNeuronOutput(Neuron *n)
{
	n_output.insert(n);
}

int Axon::input(short input_v, struct timespec &at_time)
{
	ulong n_dconnections = d_output.size();
	short dist_voltage = input_v / (n_dconnections ? n_dconnections : 1);

	for (auto i = d_output.begin(); i != d_output.end(); i++)
		(*i)->input(dist_voltage, at_time);

	/* Assume directly connected neurons indicate a "predetermined," strong correlation
	   by exciting them with a full strength input (TODO: model neurotransmitter synapses.) */
	for (auto i = n_output.begin(); i != n_output.end(); i++)
		(*i)->input(input_v, at_time);

	return dist_voltage;
}



/*
 * class Neuron
 */
Neuron::Neuron() : BrainCell(this), axon(this)
{
	dendrites.resize(1, Dendrite(this));
}

Neuron::Neuron(int xarg, int yarg) : BrainCell(this), axon(this)
{
	x = xarg;
	y = yarg;
	dendrites.resize(1, Dendrite(this));
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

int Neuron::input(short input_v, struct timespec &at_time)
{
	long long time_delta = timespec_to_ms(timespec_minus(at_time, firetime));

	if (time_delta > excited_time)
		voltage = resting_v;
	if (time_delta <= refractory_time) {
		for (uint i = 0; i < dendrites.size(); i++)
			dendrites[i].grow();
		return -1;
	}

	voltage += input_v;
	if (voltage >= action_v) {
		firetime = at_time;
		at_time = ms_to_timespec(timespec_plus(at_time, propagation_time));
		return axon.input(fire_v, at_time);
	}

	return 0;
}

int Neuron::fire()
{
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	return input(abs(voltage - action_v), time);
}

void Neuron::addDendriteOutput(Neuron *n)
{
	axon.addDendriteOutput(&n->dendrites[0]);
}

void Neuron::addNeuronOutput(Neuron *n)
{
	axon.addNeuronOutput(n);
}


/*
 * class NeuralNet
 */
NeuralNet::NeuralNet(int x, int y)
{
	neurons.resize(x, vector<Neuron>(y, Neuron()));
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			neurons[i][j].setNet(this);
			neurons[i][j].setupAxon();
			neurons[i][j].setupDendrites();
			neurons[i][j].x = i;
			neurons[i][j].y = j;
		}
}

int NeuralNet::handleDendriticBulge(Neuron *n, float bulge)
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

void NeuralNet::connectTo(NeuralNet *net)
{
	for (uint i = 0; i < this->neurons.size(); i++)
		for (uint j = 0; j < this->neurons[i].size(); j++)
			if (net->neurons.size() < i && net->neurons[i].size() < j)
				this->neurons[i][j].addNeuronOutput(&net->neurons[i][j]);
	return;
}

} // namespace
