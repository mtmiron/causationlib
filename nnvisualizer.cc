#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

//#define MAX(x, y) (x > y ? x : y)
//#define MIN(x, y) (x < y ? x : y)

using namespace TemporalNet;
using namespace cv;

Mat createImageFromNet(NeuralNet &net, int height, int width)
{
	Mat image(width, height, CV_8UC1);
	unsigned char color = 10;
	int y = net.neurons.size();
	int x = net.neurons[0].size();
	int x_scale = (x % height);
	int y_scale = (y % width);

	for (int x = 0; x < net.neurons.size(); x++)
		for (int y = 0; y < net.neurons[x].size(); y++)
			image.at<unsigned char>(x + x_scale, y + y_scale) = color;

	return image;
}
