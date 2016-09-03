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

Mat NeuralNet::createConnectionDensityImage(int width, int height)
{
	Mat image(height, width, CV_8UC3, Vec3b(0,0,0));
	int x = this->dim_x;
	int y = this->dim_y;
	int nconns = 0;
	float x_pos = 0;
	float y_pos = 0;
	uchar c = 20;//255 / neurons[0][0].max_dendrite_bulge;
	Vec3b color(c,0,c);

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			nconns = this->neurons[i][j].numberOfConnections();
			if (nconns == 0)
				continue;
			image.at<Vec3b>( PIXEL_Y(j, y_pos), PIXEL_X(i, x_pos) ) = color * nconns;
		}

	return image;
}

Mat NeuralNet::createCurrentActivityImage(int width, int height, TortoiseTime at_time, int fade_time)
{
	Mat image(height, width, CV_8UC3, Vec3b(0,0,0));
	Vec3b pixel(0,0,0);
	Vec3b color(0,0,0);
	int x = this->dim_x;
	int y = this->dim_y;
	float x_pos = 0;
	float y_pos = 0;
	uchar fire_c = 0;
	uchar input_c = 0;
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
				fire_c = 0;
			else
				fire_c = get_faded_color(fade_time, time_delta);

			time_delta = at_time - this->neurons[i][j].input_time;
			if (time_delta > oldest || time_delta < 0)
				input_c = 0;
			else
				input_c = get_faded_color(fade_time, time_delta);

			if (this->neurons[i][j].firetime == this->neurons[i][j].input_time)
				color = Vec3b(0, fire_c, 0);
			else
				color = Vec3b(input_c, 0, fire_c);
			image.at<Vec3b>( PIXEL_Y(j, y_pos), PIXEL_X(i, x_pos) ) = color;

			DEBUG_OUTPUT;
		}

	return image;
}

Mat &NeuralNet::createInputActivityImage(Mat &image, TortoiseTime at_time, int fade_time)
{
	Vec3b pixel(0,0,0);
	Vec3b color(0,0,0);
	int x = this->dim_x;
	int y = this->dim_y;
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
