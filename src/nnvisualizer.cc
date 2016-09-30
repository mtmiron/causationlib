#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;


#define PIXEL_X(neuron_x, window_x, net_x) MIN(neuron_x * ((float)window_x / net_x), window_x)
#define PIXEL_Y(neuron_y, window_y, net_y) MIN(neuron_y * ((float)window_y / net_y), window_y)


// Map the set of values in [0, max_age_in_nanoseconds) to values in [0,256)
static inline unsigned char get_faded_color(float fade_time, TortoiseTime &time_delta)
{
	long long unsigned int max;
	int diff;

	max = pow(10,9) * fade_time;
	diff = (time_delta.tv_sec * pow(10,9) + time_delta.tv_nsec) / (max / 255);
	return 255 - diff;
}


Neuron &NeuralNet::getFromWindowPosition(int Px, int Py, int Wx, int Wy)
{
	int x = round(Px / ((float)Wx / dim_x));
	int y = round(Py / ((float)Wy / dim_y));

	if (x >= 0 && x < this->dim_x && y >= 0 && y < this->dim_y)
		return neurons[x][y];
	else
		throw NeuralNetException();
}


Mat NeuralNet::createConnectionDensityImage(int width, int height)
{
	Mat image(height, width, CV_8UC3, Vec3b(0,0,0));
	uchar c = 20;
	int x = this->dim_x;
	int y = this->dim_y;
	int nconns = 0;
	Vec3b color(c,0,c);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			nconns = this->neurons[i][j].numberOfConnections();
			if (nconns == 0)
				continue;
			image.at<Vec3b>( PIXEL_Y(j, height, dim_y), PIXEL_X(i, width, dim_x) ) = color * nconns;
		}

	return image;
}


Mat NeuralNet::createCurrentActivityImage(int width, int height, TortoiseTime at_time, float fade_time, bool draw_weak_stimulation)
{
	Mat image(height, width, CV_8UC3, Vec3b(0,0,0));
	Vec3b pixel(0,0,0);
	Vec3b color(0,0,0);
	uchar fire_c = 0;
	uchar input_c = 0;
	TortoiseTime time_delta;
	struct timespec oldest = { (time_t)floor(fade_time), (long)(pow(10,9) * (fade_time - (int)fade_time)) };


	for (int i = 0; i < dim_x; i++)
		for (int j = 0; j < dim_y; j++)
		{
			time_delta = at_time - this->neurons[i][j].fire_time;
			// Don't draw neurons with future firing times, or longer than fade_time ago
			if (time_delta > oldest || time_delta < 0)
				fire_c = 0;
			else
				fire_c = get_faded_color(fade_time, time_delta);

			if (fire_c != 0) {
				if (this->neurons[i][j].fired_reason == CONCURRENT)
					color = Vec3b(0, fire_c, 0);
				else //if (this->neurons[i][j].fired_reason == SINGLE)
					color = Vec3b(0, 0, fire_c);
			}
			else if (draw_weak_stimulation) {
				time_delta = at_time - this->neurons[i][j].input_time;
				if (time_delta > oldest || time_delta < 0)
					input_c = 0;
				else
					input_c = get_faded_color(fade_time, time_delta);
				color = Vec3b(input_c, 0, 0);
			}
			else
				color = Vec3b(0, 0, 0);
			image.at<Vec3b>( PIXEL_Y(j, height, dim_y), PIXEL_X(i, width, dim_x) ) = color;
		}

	return image;
}
