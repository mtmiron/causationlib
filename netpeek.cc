#include "nnvisualizer.hh"
#include <unistd.h>
#include <iostream>

using namespace TemporalNet;
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	NeuralNet *net;
	Mat image;

	if (argc < 3)
		net = new NeuralNet(600,800);
	else
		net = new NeuralNet(atoi(argv[1]), atoi(argv[2]));


	net->setupNeurons();
	for (int loop = 0; loop < 100; loop++)
		for (int i = 100; i < 150; i++)
			for (int j = 100; j < 150; j++)
				net->neurons[i][j].input();


	image = createImageFromNet(*net, 800, 600);

	namedWindow("Neural Net: connection densities", CV_WINDOW_AUTOSIZE);
	imshow("Neural Net: connection densities", image);

	while (unsigned char key = waitKey(0))
		if (key == 27)
			break;

	return 0;
}
