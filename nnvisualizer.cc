#include "temporalnn.hh"
#include <opencv2/core.hpp>

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

Mat NeuralNet::createCurrentActivityImage(int height, int width, struct TortoiseTime &at_time)
{
	Mat image(width, height, CV_8UC3, Vec3b(0,0,0));
	Vec3b basecolor(0,0,255);
	Vec3b color(0,0,0);
	int x = this->neurons.size();
	int y = this->neurons[0].size();
	float x_pos = 0;
	float y_pos = 0;
	int diff = 0;

	x_pos = ((float)width / x);
	y_pos = ((float)height / y);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			if (this->neurons[i][j].firetime == 0 || this->neurons[i][j].firetime > at_time)
				continue;
			diff = (at_time - this->neurons[i][j].firetime).tv_nsec / pow(10,6);
			color = basecolor / (diff / this->neurons[i][j].excited_time);
			image.at<Vec3b>( MIN(round(i * x_pos), width - 1), MIN(round(j * y_pos), height - 1) ) = color;
		}

	return image;
}
