#include <unistd.h>
#include <stdio.h>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>
#include <iostream>

#define DEFAULT_STEP_SIZE 50
#define DEFAULT_NET_HEIGHT 360
#define DEFAULT_NET_WIDTH 480
#define DEFAULT_LOOP_TIME 10
#define DEFAULT_INPUT_STRENGTH 50
#define DEFAULT_LAYERS 1
#define DEFAULT_FADE_TIME 1
#define DEFAULT_PROPAGATION_TIME 1

using namespace std;
using namespace cv;


struct options {
	int stepsize = DEFAULT_STEP_SIZE;
	int height = DEFAULT_NET_HEIGHT;
	int width = DEFAULT_NET_WIDTH;
	int loop_time = DEFAULT_LOOP_TIME;
	int input_strength = DEFAULT_INPUT_STRENGTH;
	uint layers = DEFAULT_LAYERS;
	int fade_time = DEFAULT_FADE_TIME;
	int propagation_time = DEFAULT_PROPAGATION_TIME;
	bool random_step = false;
	bool freeze_connections = false;
	bool no_density_image = false;
	bool no_activity_image = false;
};

struct options opts;
extern TimeQueue BrainCell::event_queue;
extern bool BrainCell::freeze_connections;

void print_help(char *argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n"
		"\t\t-h\tThis help\n"
		"\t\t-d\tDon't draw neural connection density image\n"
		"\t\t-a\tDon't draw neural activity image\n"
		"\t\t-r\tRandom step size for neuron\n"
		"\t\t-f\tDon't self assemble neurons (freeze state)\n"
		"\t\t-p\tPropagation time (time from neuron firing to hitting next neuron)\n"
		"\t\t-F ARG\tFade time for activity image\n"
		"\t\t-s ARG\tStep size for neuron input loop\n"
		"\t\t-x ARG\tWidth of neural net\n"
		"\t\t-y ARG\tHeight of neural net\n"
		"\t\t-i ARG\tSet the input strength to each neuron\n"
		"\t\t-t ARG\tSet the wait time between wave-input iterations\n"
		"\t\t-l ARG\tNumber of neural nets\n"
		"\n", argv);
	exit(0);
}

struct options parse_args(int argc, char **argv)
{
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:dai:l:t:rfF:p:")) != -1)
	{
		switch (c) {
		case 's':
			opts.stepsize = atoi(optarg);
			break;
		case 'x':
			opts.width = atoi(optarg);
			break;
		case 'y':
			opts.height = atoi(optarg);
			break;
		case 'a':
			opts.no_activity_image = true;
			break;
		case 'd':
			opts.no_density_image = true;
			break;
		case 'i':
			opts.input_strength = atoi(optarg);
			break;
		case 't':
			opts.loop_time = atoi(optarg);
			break;
		case 'l':
			opts.layers = atoi(optarg);
			break;
		case 'r':
			opts.random_step = true;
			break;
		case 'p':
			opts.propagation_time = atoi(optarg);
			break;
		case 'F':
			opts.fade_time = atoi(optarg);
		case 'f':
			opts.freeze_connections = true;
			BrainCell::freeze_connections = true;
			break;
		case 'h':
		case '?':
		default:
			print_help(*argv);
		}
	}
	return opts;
}

void handle_keypress(uchar key)
{
	switch (key & 0xff) {
	case 'p':
		while ( ((key = waitKey(0)) & 0xff) != 'p')
			if (key == 27)
				exit(0);
		break;
	case 'c':
		cout << "Cleared " << BrainCell::event_queue.clear() << " pending action potential and/or assembly events" << endl;
		break;
	case 'f':
		BrainCell::freeze_connections = !BrainCell::freeze_connections;
		cout << "Neuron connections " << (BrainCell::freeze_connections ? "frozen" : "unfrozen") << endl;
		break;
	case 'r':
		opts.random_step = !opts.random_step;
		cout << "Random stimulation of 1 neuron per step size " << (opts.random_step ? "enabled" : "disabled") << endl;
		break;
	case 'a':
		opts.no_activity_image = !opts.no_activity_image;
		break;
	case 'd':
		opts.no_density_image = !opts.no_density_image;
		break;
	case '-':
	case '_':
		if (opts.stepsize > 1)
			opts.stepsize--;
		cout << "Step size: " << opts.stepsize << endl;
		break;
	case '+':
	case '=':
		cout << "Step size: " << ++opts.stepsize << endl;
		break;
	}
}

void interval_step(vector<NeuralNet *> nets, TortoiseTime &at_time)
{
	NeuralNet *net = nets[0];

	for (uint i = 10; i < net->neurons.size() - 10; i += opts.stepsize)
		for (uint j = 10; j < net->neurons[i].size() - 10; j += opts.stepsize)
			net->neurons[i][j].input(opts.input_strength, at_time);
}

void random_step(vector<NeuralNet *> nets, TortoiseTime &at_time)
{
	NeuralNet *net = nets[0];
	
	for (uint i = 10 + (random() % opts.stepsize); i < net->neurons.size() - 10; i += (random() % opts.stepsize))
		for (uint j = 10 + (random() % opts.stepsize); j < net->neurons[i].size() - 10; j += (random() % opts.stepsize))
			net->neurons[i][j].input(opts.input_strength, at_time);
}

void print_status()
{
	fprintf(stdout, "Neuron input strength: %dmV  Neuron step size: %d  Net height: %d  Net width: %d  "
			"Firing wave loop time: %dms  Net layers: %d  Fade time: %d  Propagation time: %dms\n"
			"Images are updated every %dms\n\n"

			"Keymap:\n"
			"ESC - exit\n"
			"'p' - toggle pause\n"
			"'r' - toggle random firing\n"
			"'d' - toggle updating of connection densities window(s)\n"
			"'a' - toggle updating of firing neurons window(s)\n"
			"'c' - clear the event queue\n"
			"'f' - freeze connections\n"
			"'-' - decrease step size\n"
			"'+' - increase step size\n",

			opts.input_strength, opts.stepsize, opts.height, opts.width,
			opts.loop_time, opts.layers, opts.fade_time, opts.propagation_time,
			opts.loop_time);
}

int main(int argc, char **argv)
{
	vector<NeuralNet *> nets;
	Mat densities_image, activity_image;
	TortoiseTime at_time;
	char windowname[256] = { 0 };

	opts = parse_args(argc, argv);
	clock_gettime(CLOCK_REALTIME, &at_time);
	srandom((int)at_time.tv_sec);
	print_status();

	for (uint i = 0; i < opts.layers; i++) {
		nets.push_back(new NeuralNet(opts.width, opts.height));
		for (int x = 0; x < opts.width; x++)
			for (int y = 0; y < opts.height; y++)
				nets[i]->neurons[x][y].setPropagationTime(opts.propagation_time);
	}
	for (uint i = 0; i < opts.layers - 1; i++)
		nets[i]->connectTo(nets[i+1]);

	for (uchar key = 0; key != 27; key = waitKey(opts.loop_time))
	{
		handle_keypress(key);

		clock_gettime(CLOCK_REALTIME, &at_time);
		if (opts.random_step)
			random_step(nets, at_time);
		else
			interval_step(nets, at_time);

#ifdef BUILD_WITH_TIMEQUEUE
		BrainCell::event_queue.applyAllUpto(at_time);
#endif

		for (uint n = 0; n < opts.layers; n++)
		{
			sprintf(windowname, "layer #%d: firing neurons", n);
			if (!opts.no_activity_image) {
				activity_image = nets[n]->createCurrentActivityImage(DEFAULT_NET_WIDTH, DEFAULT_NET_HEIGHT, at_time, opts.fade_time);
//				nets[n]->createInputActivityImage(activity_image, at_time, opts.fade_time);
				imshow(windowname, activity_image);
			}

			sprintf(windowname, "layer #%d: connection densities", n);
			if (!opts.no_density_image) {
				densities_image = nets[n]->createConnectionDensityImage(DEFAULT_NET_WIDTH, DEFAULT_NET_HEIGHT);
				imshow(windowname, densities_image);
			}
		}
	}

	return 0;
}
