#include "nnvisualizer.hh"
#include <iostream>

using namespace TemporalNet;
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	NeuralNet net(512,384);
	Mat image;
	namedWindow("Neural Net", CV_WINDOW_AUTOSIZE);

	image = createImageFromNet(net);
	imshow("Neural Net", image);

	while (unsigned char key = waitKey(0))
		if (key == 27)
			break;

	return 0;
}
