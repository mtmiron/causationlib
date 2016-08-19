#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

//#define MAX(x, y) (x > y ? x : y)
//#define MIN(x, y) (x < y ? x : y)

using namespace TemporalNet;
using namespace cv;
using namespace std;

Mat createImageFromNet(NeuralNet &net, int height, int width)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b color(1,0,1);
	int y = net.neurons.size();
	int x = net.neurons[0].size();
	int x_pos = 0;
	int y_pos = 0;
	int num = 0;

	if (x < width)
		x_pos = (width / x);
	if (y < height)
		y_pos = (height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			image.at<Vec3b>(i * x_pos, j * y_pos) = color * MAX(1,net.neurons[i][j].numberOfConnections());

	return image;
}
