#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <iostream>

using namespace TemporalNet;
using namespace cv;
using namespace std;

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

Mat NeuralNet::createCurrentActivityImage(int height, int width, struct timespec at_time)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b basecolor(255,0,0);
	Vec3b color(0,0,0);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	int diff = 0;

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);
	if (at_time.tv_sec == 0 && at_time.tv_nsec == 0)
		clock_gettime(CLOCK_REALTIME, &at_time);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			if (this->neurons[i][j].firetime.tv_sec == 0)
				continue;
			diff = (uint)(timespec_to_ms(timespec_minus(at_time, this->neurons[i][j].firetime)));
			color = (diff ? basecolor : basecolor / (diff / 100));
			image.at<Vec3b>( MIN(round(i * x_pos), width - 1), MIN(round(j * y_pos), height - 1) ) = color;
		}

	return image;
}
