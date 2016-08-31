#include <unistd.h>
#include <stdio.h>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>
#include <iostream>

#define DEFAULT_STEP_SIZE 10
#define DEFAULT_NET_HEIGHT 600
#define DEFAULT_NET_WIDTH 800
#define DEFAULT_LOOP_TIME 10
#define DEFAULT_INPUT_STRENGTH 50
#define DEFAULT_LAYERS 1

using namespace std;
using namespace cv;


struct options {
	int stepsize;
	int height;
	int width;
	int loop_time;
	int input_strength;
	uint layers;
	bool random_step;
	bool freeze_connections;
	bool no_density_image;
	bool no_activity_image;
};

struct options opts;

void print_help(char *argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n"
		"\t\t-h\tThis help\n"
		"\t\t-d\tDon't draw neural connection density image\n"
		"\t\t-a\tDon't draw neural activity image\n"
		"\t\t-r\tRandom step size for neuron\n"
		"\t\t-f\tDon't self assemble neurons (freeze state)\n"
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
	struct options opts = { DEFAULT_STEP_SIZE, DEFAULT_NET_HEIGHT, DEFAULT_NET_WIDTH,
								DEFAULT_LOOP_TIME, DEFAULT_INPUT_STRENGTH, DEFAULT_LAYERS, false };
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:dai:l:t:rf")) != -1)
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
	
	for (uint i = (rand() % opts.stepsize); i < net->neurons.size() - 10; i += (rand() % opts.stepsize))
		for (uint j = (rand() % opts.stepsize); j < net->neurons[i].size() - 10; j += (rand() % opts.stepsize))
			net->neurons[i][j].input(opts.input_strength, at_time);
}

int main(int argc, char **argv)
{
	vector<NeuralNet *> nets;
	Mat densities_image, activity_image;
	struct TortoiseTime at_time;
	char windowname[256] = { 0 };

	opts = parse_args(argc, argv);
	fprintf(stdout, "Images are updated every %dms; press ESC to exit\n"
			"Neuron input strength: %dmV  Neuron step size: %d  Net height: %d  Net width: %d  "
			"Firing wave loop time: %dms  Net layers: %d\n",
			opts.loop_time, opts.input_strength, opts.stepsize, opts.height,
			opts.width, opts.loop_time, opts.layers);

	for (uint i = 0; i < opts.layers; i++)
		nets.push_back(new NeuralNet(opts.width, opts.height));
	for (uint i = 0; i < opts.layers - 1; i++)
		nets[i]->connectTo(nets[i+1]);

	srand((int)at_time.tv_sec);
	for (uchar key = 0; key != 27; key = waitKey(opts.loop_time))
	{
		clock_gettime(CLOCK_REALTIME, &at_time);
		if (opts.random_step)
			random_step(nets, at_time);
		else
			interval_step(nets, at_time);

#ifdef WITH_TIMEQUEUE
		BrainCell::event_queue.applyAllBefore(at_time);
#endif

		for (uint n = 0; n < opts.layers; n++)
		{
			sprintf(windowname, "layer #%d: firing neurons", n);
			if (!opts.no_activity_image) {
				activity_image = nets[n]->createCurrentActivityImage(480, 360, at_time);
				imshow(windowname, activity_image);
			}

			sprintf(windowname, "layer #%d: connection densities", n);
			if (!opts.no_density_image) {
				densities_image = nets[n]->createConnectionDensityImage(480, 360);
				imshow(windowname, densities_image);
			}
		}
	}

	return 0;
}
