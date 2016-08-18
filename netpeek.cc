#include "nnvisualizer.hh"
#include <iostream>

using namespace TemporalNet;
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	NeuralNet net;
	Mat image;

	image = createImageFromNet(net);
	imshow("neural net", image);

	while (unsigned char key = waitKey(0))
		if (key == 27)
			break;

	return 0;
}
