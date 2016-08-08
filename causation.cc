#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <cstdio>
#include <iostream>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <opencv2/highgui.hpp>
#include <time.h>
#include <math.h>

#define SHOW_IMAGES 0
#define REPOSITION_CURSOR_LAST_LINE std::cout << '\x08' << '\x0D'

/*
"0: CAP_OPENNI_DEPTH_MAP 	Depth values in mm (CV_16UC1)",
"1: CAP_OPENNI_POINT_CLOUD_MAP 	XYZ in meters (CV_32FC3)",
"2: CAP_OPENNI_DISPARITY_MAP 	Disparity in pixels (CV_8UC1)",
"3: CAP_OPENNI_DISPARITY_MAP_32F 	Disparity in pixels (CV_32FC1)",
"4: CAP_OPENNI_VALID_DEPTH_MASK 	CV_8UC1",
"5: CAP_OPENNI_BGR_IMAGE 	Data given from RGB image generator",
"6: CAP_OPENNI_GRAY_IMAGE 	Data given from RGB image generator",
"7: CAP_OPENNI_IR_IMAGE 	Data given from IR image generator",
*/

static const char* channel_map[] = {
	"CAP_OPENNI_DEPTH_MAP        ",
	"CAP_OPENNI_POINT_CLOUD_MAP  ",
	"CAP_OPENNI_DISPARITY_MAP    ",
	"CAP_OPENNI_DISPARITY_MAP_32F",
	"CAP_OPENNI_VALID_DEPTH_MASK ",
	"CAP_OPENNI_BGR_IMAGE        ",
	"CAP_OPENNI_GRAY_IMAGE       ",
	"CAP_OPENNI_IR_IMAGE         ",
};

/*
 * Rudimentary visual change detection:
 *
 * An image frame is a matrix, each element corresponding to an
 * intensity value at that pixel.  The norm of a matrix is
 * the equivalent of the magnitude of that matrix; thus,
 * the norm of an image frame is the equivalent of the intensity
 * of the pixels.
 *
 * With a Kinect making depth/IR values available, this works
 * surprisingly well for almost no computational cost.
 */
bool visual_iter(struct timespec &event)
{
	static cv::VideoCapture cam(cv::CAP_OPENNI);
	static cv::Mat framebuf;
	static std::vector<cv::Mat> vidFrame;
	static std::vector<float> old_norm, cur_norm, delta;
	bool event_took_place = false;

	if (!cam.isOpened())
		throw "failed to open camera";

	cam.grab();
	for (int i = 0; i < 7; i++)
	{
		if (vidFrame.size() < i+1) {
			vidFrame.push_back(cv::Mat());
			old_norm.push_back(0.0);
			cur_norm.push_back(0.0);
			delta.push_back(0.0);
		}
		cam.retrieve(framebuf, i);
		cur_norm[i] = cv::norm(framebuf);
		if (SHOW_IMAGES) {
			framebuf.copyTo(vidFrame[i]);
			imshow(channel_map[i], vidFrame[i]);
		}
	}
	if (SHOW_IMAGES)
		cv::waitKey(10);

	for (int i = 0; i < vidFrame.size(); i++) {
		delta[i] = abs(cur_norm[i] - old_norm[i]);
		std::cout << channel_map[i] << "\t" << cur_norm[i] << " \t " << delta[i] << std::endl;
	}

	for (int i = 0; i < vidFrame.size(); i++) {
		old_norm[i] = cur_norm[i];
		if ( (cur_norm[i] * 0.10) <= delta[i] )
			event_took_place = !clock_gettime(CLOCK_REALTIME, &event);
		REPOSITION_CURSOR_LAST_LINE;
	}

	return event_took_place;
}

int main(int argc, char **argv)
{
	int pid = 0;
	struct timespec event_times[1] = { 0 };

	pid = getpid();
	if (setpriority(PRIO_PROCESS, pid, 20) == -1 && errno != 0)
		throw strerror(errno);

	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(5);
	std::cout << "Norm of image matrixes: " << std::endl;

	while (true)
	{
		if (visual_iter(event_times[0])) {
			REPOSITION_CURSOR_LAST_LINE;
			std::cout << "visual event took place at " << event_times[0].tv_sec << "." << event_times[0].tv_nsec << std::endl;
		}
		usleep(40000);
	}
	return 0;
}
