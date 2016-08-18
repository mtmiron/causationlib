#include "temporalnn.hh"


namespace TemporalNet {
using namespace std;


/*
 * class Dendrite
 */
Dendrite::Dendrite(Neuron *owner, short delay, short seek, short cluster)
		: delaytime(delay), seekfactor(seek), clusterfactor(cluster),
		  bulge(0)
{
	clock_gettime(CLOCK_REALTIME, &time);
	neuron = owner;
}

int Dendrite::fire(short input_v)
{
	neuron->fire(input_v);
}

int Dendrite::grow(short input_v)
{
	bulge += input_v * seekfactor;
	return bulge;
}


/*
 * class Axon
 */
Axon::Axon(short vscls) : vesicles(vscls)
{ }

int Axon::fire(short input_v)
{
	long n_dconnections = d_output.size();
	long n_nconnections = n_output.size();
	short dist_voltage = input_v / (n_dconnections + n_nconnections);

	if (n_dconnections == 0 && n_nconnections == 0)
		return -1;
	for (int i = 0; i < n_dconnections; i++)
		d_output[i]->fire(dist_voltage);
}

/*
 * class Neuron
 */
Neuron::Neuron(NeuralNet *owner, short reftime, short excitetime, short refv, short rest_v,
		short act_v, short firev) : refractory_time(reftime),excited_time(excitetime),
		  refractory_v(refv), resting_v(rest_v), action_v(act_v), fire_v(firev)
{
	net = owner;
	dendrites.push_back(Dendrite(this));
	clock_gettime(CLOCK_REALTIME, &time);
}


int Neuron::fire(short input_v)
{
	static short voltage = resting_v;
	long time_delta = 0;
	struct timespec nowtime;

	clock_gettime(CLOCK_REALTIME, &nowtime);
	time_delta = nano_to_milli(nowtime.tv_nsec) - nano_to_milli(time.tv_nsec);

	if (time_delta > excited_time)
		voltage = resting_v;
	voltage += input_v;
	if (time_delta <= refractory_time)
		return 0;

	if (voltage >= fire_v)
		if (axon.fire(fire_v) < 0)
			for (int i = 0; i < dendrites.size(); i++)
				dendrites[i].grow(input_v);

	return 0;
}

/*
 * class NeuralNet
 */
NeuralNet::NeuralNet(int x, int y)
{
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			neurons[i].push_back(Neuron(this));
}

} // namespace
