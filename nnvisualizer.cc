#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace TemporalNet;
using namespace cv;
using namespace std;

Mat createImageFromNet(NeuralNet &net, int height, int width)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b color(1,0,1);
	int x = net.neurons.size();
	int y = net.neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	int num = 0;

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			image.at<Vec3b>( MIN(round(i * x_pos), width - 1), MIN(round(j * y_pos), height - 1) ) = color * MAX(1,net.neurons[i][j].numberOfConnections());

	return image;
}
