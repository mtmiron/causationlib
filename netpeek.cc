#include <unistd.h>
#include <stdio.h>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>

#define DEFAULT_STEP_SIZE 2
#define DEFAULT_NET_HEIGHT 480
#define DEFAULT_NET_WIDTH 320

using namespace TemporalNet;
using namespace std;
using namespace cv;


struct options {
	int stepsize;
	int height;
	int width;
	int input_strength;
	bool no_density_image;
	bool no_activity_image;
};


void print_help(char **argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n\
\t\t-h\tThis help\n\
\t\t-d\tDon't draw neural connection density image\n\
\t\t-a\tDon't draw neural activity image\n\
\t\t-x OPT\tWidth of neural net\n\
\t\t-y OPT\tHeight of neural net\n\
\t\t-s OPT\tStep size for neuron input loop\n\
\t\t-i OPT\tSet the input strength to each neuron\n\
\n", *argv);
	exit(0);
}

struct options parse_args(int argc, char **argv)
{
	struct options options = { DEFAULT_STEP_SIZE, DEFAULT_NET_HEIGHT, DEFAULT_NET_WIDTH };
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:dai:")) != -1)
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
		}
	}
	return options;
}

int main(int argc, char **argv)
{
	NeuralNet *net;
	Mat densities_image, activity_image;
	struct options options = parse_args(argc, argv);

	fprintf(stdout, "Key bindings: ESC == exit :)\n"
			"Neuron input strength: %d  Neuron step size: %d  Net height: %d  Net width: %d\n",
			options.input_strength, options.stepsize, options.height, options.width);

	net = new NeuralNet(options.width, options.height);
	net->setupNeurons();

	for (uchar key = 0; key != 27; key = waitKey(100))
	{
		for (int i = 10; i < net->neurons.size() - 10; i += options.stepsize)
			for (int j = 10; j < net->neurons[i].size() - 10; j += options.stepsize)
				net->neurons[i][j].input(options.input_strength);

		if (!options.no_density_image) {
			densities_image = net->createConnectionDensityImage(800, 600);
			imshow("connection densities", densities_image);
		}
		if (!options.no_activity_image) {
			activity_image = net->createCurrentActivityImage(800, 600);
			imshow("current activity", activity_image);
		}
	}

	return 0;
}
