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


// Map the set of values in [0, max_age_in_nanoseconds) to values in [0,256)
static inline unsigned char get_faded_color(int fade_time, TortoiseTime &time_delta)
{
	long long unsigned int max;
	int diff;

	max = pow(10,9) * fade_time;
	diff = (time_delta.tv_sec * pow(10,9) + time_delta.tv_nsec) / (max / 255);
	return 255 - diff;
}

Mat NeuralNet::createConnectionDensityImage(int height, int width)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b color(5,0,5);
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
			image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) ) = color * nconns;
		}

	return image;
}

Mat NeuralNet::createCurrentActivityImage(int height, int width, TortoiseTime at_time, int fade_time)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b pixel(0,0,0);
	Vec3b color(0,0,0);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	TortoiseTime time_delta;
	struct timespec oldest = { fade_time, 0 };

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			time_delta = at_time - this->neurons[i][j].firetime;
			// Don't draw neurons with future firing times, or longer than fade_time ago
			if (time_delta > oldest || time_delta < 0)
				continue;

			color = Vec3b(0,0,get_faded_color(fade_time, time_delta));
			pixel = image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) );
			if (pixel[2] == 0)
				image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) ) = color;

			DEBUG_OUTPUT;
		}

	return image;
}

Mat &NeuralNet::createInputActivityImage(Mat &image, TortoiseTime at_time, int fade_time)
{
	Vec3b pixel(0,0,0);
	Vec3b color(0,0,0);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	TortoiseTime time_delta;
	struct timespec oldest = { fade_time, 0 };
	int width = image.size().width;
	int height = image.size().height;

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			time_delta = at_time - this->neurons[i][j].input_time;
			if (time_delta > oldest || time_delta < 0)
				continue;

			color = Vec3b(get_faded_color(fade_time, time_delta), 0, 0);
			pixel = image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) );
			if (pixel[2] < get_faded_color(fade_time, time_delta))
				image.at<Vec3b>( PIXEL_X(i, x_pos), PIXEL_Y(j, y_pos) ) = color;

			DEBUG_OUTPUT;
		}

	return image;
}