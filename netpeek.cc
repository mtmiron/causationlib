#include <unistd.h>
#include <stdio.h>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>

#define DEFAULT_STEP_SIZE 10
#define DEFAULT_NET_HEIGHT 600
#define DEFAULT_NET_WIDTH 800
#define DEFAULT_LOOP_TIME 100
#define DEFAULT_INPUT_STRENGTH 50

using namespace TemporalNet;
using namespace std;
using namespace cv;


struct options {
	int stepsize;
	int height;
	int width;
	int loop_time;
	int input_strength;
	bool no_density_image;
	bool no_activity_image;
};


void print_help(char **argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n"
		"\t\t-h\tThis help\n"
		"\t\t-d\tDon't draw neural connection density image\n"
		"\t\t-a\tDon't draw neural activity image\n"
		"\t\t-x OPT\tWidth of neural net\n"
		"\t\t-y OPT\tHeight of neural net\n"
		"\t\t-s OPT\tStep size for neuron input loop\n"
		"\t\t-i OPT\tSet the input strength to each neuron\n"
		"\t\t-l OPT\tSet the wait time between wave-input iterations\n"
		"\n", *argv);
	exit(0);
}

struct options parse_args(int argc, char **argv)
{
	struct options options = { DEFAULT_STEP_SIZE, DEFAULT_NET_HEIGHT, DEFAULT_NET_WIDTH,
								DEFAULT_LOOP_TIME, DEFAULT_INPUT_STRENGTH };
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:dai:l:")) != -1)
	{
		switch (c) {
		case 's':
			options.stepsize = atoi(optarg);
			break;
		case 'h':
			print_help(argv);
			break;
		case 'x':
			options.width = atoi(optarg);
			break;
		case 'y':
			options.height = atoi(optarg);
			break;
		case 'a':
			options.no_activity_image = true;
			break;
		case 'd':
			options.no_density_image = true;
			break;
		case 'i':
			options.input_strength = atoi(optarg);
			break;
		case 'l':
			options.loop_time = atoi(optarg);
			break;
		}
	}
	return options;
}

int main(int argc, char **argv)
{
	NeuralNet *net;
	Mat densities_image, activity_image;
	struct timespec at_time = { 0 };
	struct options options = parse_args(argc, argv);

	fprintf(stdout, "Images are updated every %dms; press ESC to exit\n"
			"Neuron input strength: %dmV  Neuron step size: %d  Net height: %d  Net width: %d  "
			"Firing wave loop time: %dms\n",
			options.loop_time, options.input_strength, options.stepsize, options.height,
			options.width, options.loop_time);

	net = new NeuralNet(options.width, options.height);
	net->setupNeurons();

	for (uchar key = 0; key != 27; key = waitKey(options.loop_time))
	{
		clock_gettime(CLOCK_REALTIME, &at_time);

		for (uint i = 10; i < net->neurons.size() - 10; i += options.stepsize)
			for (uint j = 10; j < net->neurons[i].size() - 10; j += options.stepsize)
				net->neurons[i][j].input(options.input_strength, at_time);

		if (!options.no_activity_image) {
			activity_image = net->createCurrentActivityImage(800, 600, at_time);
			imshow("firing neurons", activity_image);
		}

		if (!options.no_density_image) {
			densities_image = net->createConnectionDensityImage(800, 600);
			imshow("connection densities", densities_image);
		}
	}

	return 0;
}
