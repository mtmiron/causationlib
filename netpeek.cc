#include <unistd.h>
#include <iostream>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>


using namespace TemporalNet;
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	NeuralNet *net;
	Mat densities_image, activity_image;

	if (argc < 3)
		net = new NeuralNet(600,800);
	else
		net = new NeuralNet(atoi(argv[1]), atoi(argv[2]));


	net->setupNeurons();
	namedWindow("current activity", CV_WINDOW_AUTOSIZE);
	namedWindow("connection densities", CV_WINDOW_AUTOSIZE);
	for (uchar key = 0; key != 27; key = waitKey(1))
	{
		for (int i = 100; i < 110; i++)
			for (int j = 100; j < 110; j++)
				net->neurons[i][j].input();

		densities_image = net->createConnectionDensityImage(800, 600);
		imshow("connection densities", densities_image);

		activity_image = net->createCurrentActivityImage(800, 600);
		imshow("current activity", activity_image);
	}

	return 0;
}
