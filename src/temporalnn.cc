#include "temporalnn.hh"
#include <iostream>
#include <mutex>


/*
 * Current design requirements (to maintain nnvisualizer.cc parameters) are as
 * follows (the result of these two leave /input_time/ and /fire_time/ equal
 * if sub-threshold input triggered the action potential, and different if
 * a single input strong enough to trigger the neuron to fire was received.)
 *
 *
 *  - /input_time/ must be set ONLY when sub-action-potential input is received
 *
 *  - /fire_time/ must ALWAYS be set when an action potential (firing) is triggered
 *
 *
 */


#define BIND(T) bind(&T::bound_input, this, input_v, at_time)

using namespace std;


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


#ifdef BUILD_WITH_TIMEQUEUE
int Dendrite::bound_input(short input_v, TortoiseTime at_time)
#else
int Dendrite::input(short input_v, TortoiseTime at_time)
#endif
{
	return neuron->input(input_v, at_time);
}


#ifdef BUILD_WITH_TIMEQUEUE
int Dendrite::input(short input_v, TortoiseTime at_time)
{
	event_queue.insert(at_time, BIND(Dendrite));
	return 0;
}
#endif


int Dendrite::grow()
{
	if (freeze_connections)
		return -1;

	if (bulge < max_dendrite_bulge) {
		bulge += 1 * seekfactor;
		return this->neuron->net->growDendrite(this->neuron, bulge);
	}
	else
		return -1;
}


int Dendrite::shrink(Neuron *n)
{
	if (freeze_connections)
		return -1;

	n->axon.d_output.clear();
	bulge -= 1 * seekfactor;
	if (bulge < 0) {
		bulge = 0;
		return 0;
	}

	return this->neuron->net->growDendrite(this->neuron, bulge);
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


#ifdef BUILD_WITH_TIMEQUEUE
int Axon::bound_input(short input_v, TortoiseTime at_time)
#else
int Axon::input(short input_v, TortoiseTime at_time)
#endif
{
	at_time = at_time + propagation_time;
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


#ifdef BUILD_WITH_TIMEQUEUE
int Axon::input(short input_v, TortoiseTime at_time)
{
	event_queue.insert(at_time, BIND(Axon));
	return 0;
}
#endif


/*
 * class Neuron
 */
Neuron::Neuron() : BrainCell(this), axon(this)
{
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


#ifdef BUILD_WITH_TIMEQUEUE
int Neuron::bound_input(short input_v, TortoiseTime at_time)
#else
int Neuron::input(short input_v, TortoiseTime at_time)
#endif
{
	TortoiseTime time_delta(at_time - fire_time);

	if (time_delta <= refractory_time) {
		this->refracting = true;
		return 0;
	}
	this->refracting = false;

	if (input_v + resting_v >= action_v) {
		fire_time = at_time;
		voltage = resting_v;
		int ret = axon.input(fire_v, at_time);
		if (shrink_dendrites)
			for (uint i = 0; i < dendrites.size(); i++)
				dendrites[i].shrink(this);
		return ret;
	} else if (at_time - input_time > excited_time) {
		voltage = resting_v;
	}

	input_time = at_time;
	voltage += input_v;
	if ( voltage >= action_v ) {
		fire_time = at_time;
		voltage = resting_v;
		int ret = axon.input(fire_v, at_time);
		if (shrink_dendrites)
			for (uint i = 0; i < dendrites.size(); i++)
				dendrites[i].shrink(this);
		return ret;
	} else {
		for (uint i = 0; i < dendrites.size(); i++)
			dendrites[i].grow();
		return 1;
	}

	return 0;
}


#ifdef BUILD_WITH_TIMEQUEUE
int Neuron::input(short input_v, TortoiseTime at_time)
{
	event_queue.insert(at_time, BIND(Neuron));
	return 0;
}
#endif


int Neuron::fire()
{
	TortoiseTime time;

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


void Neuron::setPropagationTime(int prop)
{
	this->propagation_time = prop;
	axon.propagation_time = prop;
	for (uint i = 0; i < dendrites.size(); i++)
		dendrites[i].propagation_time = prop;
}


void Neuron::setExcitedTime(unsigned short time)
{
	this->excited_time = time;
}


void Neuron::setRefractoryTime(unsigned short time)
{
	this->refractory_time = time;
}


void Neuron::setMaxDendriteConnections(unsigned int max)
{
	this->max_dendrite_bulge = max;
	axon.max_dendrite_bulge = max;
	for (uint i = 0; i < dendrites.size(); i++)
		dendrites[i].max_dendrite_bulge = max;
}


ostream &operator<<(ostream &os, Neuron &neuron)
{
	os << "{(" << neuron.x << "," << neuron.y << "); " << "input_t = " << neuron.input_time
		<< "; fire_t = " << neuron.fire_time << "; voltage = " << neuron.voltage << "mV; propagation_t = "
		<< neuron.axon.propagation_time << "ms; refractory_t = " << neuron.refractory_time << "ms; "
		<< "excited_t = " << neuron.excited_time << "ms; " << "num connections = " << neuron.numberOfConnections()
		<< "; " << "refracting = " << neuron.refracting << "}";
	return os;
}


/*
 * class NeuralNet
 */
NeuralNet::NeuralNet(int x, int y)
{
	dim_x = x;
	dim_y = y;
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


int NeuralNet::growDendrite(Neuron *n, float bulge)
{
	int xpos = n->x;
	int ypos = n->y;
	int scale = round(bulge);

	if (scale >= dim_x && scale >= dim_y)
		return 0;

	for (int i = scale / -2; i <= 0; i++)
		for (int j = scale / -2; j < 0; j++)
			if (xpos + i > 0 && ypos + j > 0)
					neurons[xpos + i][ypos + j].addDendriteOutput(n);

	for (int i = 0; i < scale / 2; i++)
		for (int j =1; j < scale / 2; j++)
			if (xpos + i < dim_x && ypos + j < dim_y)
					neurons[xpos + i][ypos + j].addDendriteOutput(n);

	return 0;
}


void NeuralNet::connectTo(NeuralNet *net)
{
	for (uint i = 0; i < this->neurons.size(); i++)
		for (uint j = 0; j < this->neurons[i].size(); j++)
			this->neurons[i][j].addNeuronOutput(&net->neurons[i][j]);
	return;
}
