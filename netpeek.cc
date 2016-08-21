#include "nnvisualizer.hh"
#include <iostream>

using namespace TemporalNet;
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	NeuralNet net;
	Mat image;

	if (argc < 3)
		net = NeuralNet(512,384);
	else
		net = NeuralNet(atoi(argv[1]), atoi(argv[2]));

	namedWindow("Neural Net", CV_WINDOW_AUTOSIZE);

	net.setupNeurons();
	image = createImageFromNet(net, 800, 600);
	imshow("Neural Net", image);

	while (unsigned char key = waitKey(0))
		if (key == 27)
			break;

	return 0;
}
