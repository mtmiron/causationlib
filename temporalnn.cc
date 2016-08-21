#include "temporalnn.hh"


namespace TemporalNet {
using namespace std;


long nano_to_milli(long nano)

{
	return nano / pow(10, 6);
}

long milli_to_nano(long milli)
{
	return milli * pow(10, 6);
}


/*
 * class Dendrite
 */
Dendrite::Dendrite(short delay, float seek, short cluster)
		: delaytime(delay), seekfactor(seek), clusterfactor(cluster),
		  bulge(0)
{
	clock_gettime(CLOCK_REALTIME, &time);
}

Dendrite *Dendrite::setNeuron(Neuron *owner)
{
	neuron = owner;
	return this;
}

int Dendrite::fire(short input_v)
{
	return neuron->fire(input_v);
}

int Dendrite::grow(short input_v)
{
	bulge += input_v * seekfactor;
	return neuron->handleDendriticBulge(bulge);
}


/*
 * class Axon
 */
Axon::Axon(short vscls) : vesicles(vscls)
{
	clock_gettime(CLOCK_REALTIME, &time);
}

Neuron *Axon::setNeuron(Neuron *owner)
{
	return neuron = owner;
}

int Axon::numberOfConnections()
{
	return d_output.size() + n_output.size();
}

int Axon::fire(short input_v)
{
	long n_dconnections = d_output.size();
	long n_nconnections = n_output.size();
	short dist_voltage = input_v / n_dconnections;

	if (n_dconnections == 0 && n_nconnections == 0)
		return -1;
	for (int i = 0; i < n_dconnections; i++)
		d_output[i]->fire(dist_voltage);
	/*
	 * Assume directly connected neurons indicate a "predetermined," strong correlation
	 * by exciting them with a full strength input (TODO: model neurotransmitter synapses.)
	 */
	for (int i = 0; i < n_nconnections; i++)
		n_output[i]->fire(input_v);
}

/*
 * class Neuron
 */
Neuron::Neuron(short reftime, short excitetime, short refv, short rest_v,
		short act_v, short firev) : refractory_time(reftime),excited_time(excitetime),
		  refractory_v(refv), resting_v(rest_v), action_v(act_v), fire_v(firev)
{
	dendrites.push_back(Dendrite());
	clock_gettime(CLOCK_REALTIME, &time);
}

Neuron *Neuron::setNet(NeuralNet *owner)
{
	this->net = owner;
	return this;
}

Neuron *Neuron::setupDendrites()
{
	for (int i = 0; i < dendrites.size(); i++)
		dendrites[i].setNeuron(this);
	return this;
}

Neuron *Neuron::setupAxon()
{
	return axon.setNeuron(this);
}

int Neuron::numberOfConnections()
{
	return 50;
	return axon.numberOfConnections();
}

int Neuron::fire(short input_v)
{
	static short voltage = resting_v;
	long time_delta = 0;
	struct timespec nowtime;

	clock_gettime(CLOCK_REALTIME, &nowtime);
	time_delta = nano_to_milli(nowtime.tv_nsec) - nano_to_milli(time.tv_nsec);

	// TODO: don't make assumptions about CPU capabilities to handle real-time events.
	if (time_delta > excited_time)
		voltage = resting_v;
	voltage += input_v;
	if (time_delta <= refractory_time)
		return -1;

	if (voltage >= fire_v)
		axon.fire(fire_v);
	else {
		for (int i = 0; i < dendrites.size(); i++)
			dendrites[i].grow(input_v);
	}	
	return 0;
}

int Neuron::handleDendriticBulge(float bulge)
{
	return net->handleDendriticBulge(this, bulge);
}

/*
 * class NeuralNet
 */
NeuralNet::NeuralNet(int x, int y)
{
	neurons.reserve(x);
	for (int i = 0; i < x; i++)
	{
		vector<Neuron> v;
		v.reserve(y);
		neurons.push_back(v);
		for (int j = 0; j < y; j++)
			neurons[i].push_back(Neuron());
	}
}

int NeuralNet::handleDendriticBulge(Neuron *n, float bulge)
{
}

void NeuralNet::setupNeurons()
{
	for (int i = 0; i < neurons.size(); i++)
		for (int j = 0; j < neurons[i].size(); j++)
		{
			neurons[i][j].setNet(this);
			neurons[i][j].setupAxon();
			neurons[i][j].setupDendrites();
		}
}

} // namespace
