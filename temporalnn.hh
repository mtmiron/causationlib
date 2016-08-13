#include <vector>
#include <map>
#include <math.h>
#include <time.h>


namespace TemporalNet {

using namespace std;


class Axon;
class Dendrite;
class Neuron;


long nano_to_milli(long nano)
{
	return nano / pow(10, 6);
}

long milli_to_nano(long milli)
{
	return milli * pow(10, 6);
}


class Dendrite
{
  private:
	short delaytime;
	short seekfactor;
	short clusterfactor;
	struct timespec time;

  public:
	Dendrite(short delay = 0, short seek = 1, short cluster = 1)
		: delaytime(delay), seekfactor(seek), clusterfactor(cluster)
	{ }

	~Dendrite();

	void fire(short input_v)
	{
		// TODO: figure out how to make dendrites grow based on input voltage events
	}
};


class Axon
{
  private:
	short vesicles;
	struct timespec time;

	vector<Dendrite *> d_output;

  public:
	Axon(short vscls = 0) : vesicles(vscls) {}

	~Axon();

	void fire(short input_v)
	{
		long n_dconnections = d_output.size();
		short dist_voltage = input_v / n_dconnections;

		for (int i = 0; i < n_dconnections; i++)
			d_output[i]->fire(dist_voltage);
	}
};


class Neuron
{
  private:
	short refractory_time = 2;
	short excited_time = 20;
	short refractory_v = -50;
	short resting_v = -80;
	short action_v = -30;
	short fire_v = 50;
	struct timespec time;

  protected:
	Axon axon;
	vector<Dendrite> dendrites;

  public:
	Neuron(short reftime = 50, short excitetime = 20, short refv = -50,
			short rest_v = -80, short act_v = -30, short firev = 50)
		: refractory_time(reftime), excited_time(excitetime),
		  refractory_v(refv), resting_v(rest_v), action_v(act_v),
		  fire_v(firev), axon(), dendrites()
	{
		clock_gettime(CLOCK_REALTIME, &time);
	}

	~Neuron();

	void fire(short input_v)
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
			return;

		if (voltage >= fire_v)
			axon.fire(fire_v);

		return;
	}
};

class NeuralNet
{
  private:
	vector<Neuron> neuron;

  public:
	NeuralNet();
	~NeuralNet();

};



} // namespace
