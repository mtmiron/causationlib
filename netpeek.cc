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


static struct options {
	int stepsize;
	int height;
	int width;
} options;


void print_help(char **argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n\
\t\t-h\tThis help\n\
\t\t-s\tStep size for neuron input loop\n\
\t\t-x\tWidth of neural net\n\
\t\t-y\tHeight of neural net\n\
\n", *argv);
	exit(0);
}

struct options parse_args(int argc, char **argv)
{
	options = { DEFAULT_STEP_SIZE, DEFAULT_NET_HEIGHT, DEFAULT_NET_WIDTH };
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:")) != -1)
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
		}
	}
	return options;
}

int main(int argc, char **argv)
{
	NeuralNet *net;
	Mat densities_image, activity_image;

	options = parse_args(argc, argv);
	fprintf(stdout, "Neuron step size: %d  Net height: %d  Net width: %d\n** HIT ESCAPE TO EXIT :) **\n",
			options.stepsize, options.height, options.width);

	net = new NeuralNet(options.width, options.height);
	net->setupNeurons();

	namedWindow("current activity", CV_WINDOW_AUTOSIZE);
	namedWindow("connection densities", CV_WINDOW_AUTOSIZE);
	for (uchar key = 0; key != 27; key = waitKey(1))
	{
		for (int i = 10; i < net->neurons.size() - 10; i += options.stepsize)
			for (int j = 10; j < net->neurons[i].size() - 10; j += options.stepsize)
				net->neurons[i][j].input();

		densities_image = net->createConnectionDensityImage(800, 600);
		imshow("connection densities", densities_image);

		activity_image = net->createCurrentActivityImage(800, 600);
		imshow("current activity", activity_image);
	}

	return 0;
}
