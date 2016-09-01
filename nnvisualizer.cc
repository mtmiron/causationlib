#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#if 0
#define DEBUG_OUTPUT \
			cout << "(" << i << "," << j << ")" " --> "; \
			cout << "(" << MIN(round(i * x_pos), width - 1) << "," << MIN(round(j * y_pos), height - 1) << ")\t== {" << time_delta << ": " << diff << "} " << color << endl;
#else
#define DEBUG_OUTPUT
#endif

#define PIXEL_X(neuron_x, x_scale) MIN(round(neuron_x * x_scale), width - 1)
#define PIXEL_Y(neuron_y, y_scale) MIN(round(neuron_y * y_scale), height - 1)


Mat NeuralNet::createConnectionDensityImage(int height, int width)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b color(1,0,1);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	int nconns = 0;
	float x_pos = 0;
	float y_pos = 0;

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			nconns = this->neurons[i][j].numberOfConnections();
			if (nconns == 0)
				continue;
			image.at<Vec3b>( MIN(round(i * x_pos), width - 1), MIN(round(j * y_pos), height - 1) ) = color * nconns;
		}

	return image;
}

Mat NeuralNet::createCurrentActivityImage(int height, int width, TortoiseTime at_time, int fade_time)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b pixel(0,0,255);
	Vec3b color(0,0,0);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	long long unsigned int diff = 0;
	long long unsigned int max = pow(10,9) * fade_time;
	TortoiseTime time_delta;
	struct timespec oldest = { fade_time, 0 };

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			time_delta = at_time - this->neurons[i][j].firetime;
			if (time_delta > oldest || time_delta < 0)
				continue;

			diff = (time_delta.tv_sec * pow(10,9) + time_delta.tv_nsec) / (max / 255);
			color = Vec3b(0,0,255 - diff);
			pixel = image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) );
			if (pixel[2] == 0)
				image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) ) = color;

			DEBUG_OUTPUT;
		}

	return image;
}
