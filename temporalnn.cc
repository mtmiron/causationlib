#include "temporalnn.hh"
#include <algorithm>
#include <iostream>

namespace TemporalNet {
using namespace std;


long long nano_to_milli(long long nano)

{
	return nano / pow(10, 6);
}

long long milli_to_nano(long long milli)
{
	return milli * pow(10, 6);
}

long long timespec_minus(struct timespec &time1, struct timespec &time2)
{
	long long ret;

	ret = time1.tv_sec - time2.tv_sec;
	ret *= 1000;
	ret += (nano_to_milli(time1.tv_nsec) - nano_to_milli(time2.tv_nsec));

	return ret;
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

int Dendrite::fire(short input_v)
{
	return neuron->fire(input_v);
}

int Dendrite::grow(short input_v)
{
/* TODO: design it properly, genius.
	bulge += input_v * seekfactor;
*/
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
//	clock_gettime(CLOCK_REALTIME, &firetime);
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

int Axon::fire(short input_v)
{
	ulong n_dconnections = d_output.size();
	ulong n_nconnections = n_output.size();
	short dist_voltage = input_v / (n_dconnections ? n_dconnections : 1);

	if (n_dconnections == 0 && n_nconnections == 0)
		return -1;
	for (uint i = 0; i < n_dconnections; i++)
		d_output[i]->fire(dist_voltage);
	/*
	 * Assume directly connected neurons indicate a "predetermined," strong correlation
	 * by exciting them with a full strength input (TODO: model neurotransmitter synapses.)
	 */
	for (uint i = 0; i < n_nconnections; i++)
		n_output[i]->fire(input_v);

	return 0;
}



/*
 * class Neuron
 */
Neuron::Neuron(short reftime, short excitetime, short refv, short rest_v,
		short act_v, short firev) : refractory_time(reftime),excited_time(excitetime),
		  refractory_v(refv), resting_v(rest_v), action_v(act_v), fire_v(firev),
		  axon()
{
	dendrites.push_back(Dendrite());
//	clock_gettime(CLOCK_REALTIME, &firetime);
}

Neuron::Neuron(int xarg, int yarg)
{
	x = xarg;
	y = yarg;
	dendrites.push_back(Dendrite());
//	clock_gettime(CLOCK_REALTIME, &firetime);
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

int Neuron::fire(short input_v)
{
	static short voltage = resting_v;
	uint time_delta = 0;
	struct timespec nowtime;

	clock_gettime(CLOCK_REALTIME, &nowtime);
	time_delta = (uint)timespec_minus(nowtime, firetime);

	// TODO: don't make assumptions about CPU capabilities to handle real-time events.
	if (time_delta > excited_time)
		voltage = resting_v;
	if (time_delta <= refractory_time)
		return -1;
	voltage += input_v;

	if (voltage >= action_v) {
		axon.fire(fire_v);
		firetime.tv_sec = nowtime.tv_sec;
		firetime.tv_nsec = nowtime.tv_nsec;
	} else {
		for (uint i = 0; i < dendrites.size(); i++)
			dendrites[i].grow(input_v);
	}
	return 0;
}

void Neuron::addConnection(Neuron *n)
{
	axon.addDendriteOutput(&n->dendrites[0]);
}

/* 
 * The point is that if we're firing without input, then
 * we're out of sync and useless.  The more we fire independently,
 * the more we seek the input of other action potentials to sync
 * with.
 */
int Neuron::handleDendriticBulge(double bulge)
{
	return net->handleDendriticBulge(this, bulge);
}

int Neuron::input(short input_v)
{
	return fire(input_v);
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

	for (int i = 1; i <= scale; i++)
		for (int j = 1; j <= scale; j++)
			if (xpos + i < neurons.size() && ypos + j < neurons[xpos + i].size())
				neurons[xpos + i][ypos + j].addConnection(n);

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

} // namespace
