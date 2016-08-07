#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <cstdio>
#include <iostream>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define REPOSITION_CURSOR_LAST_LINE std::cout << '\x08' << '\x0D'


int main(int argc, char **argv)
{
	cv::VideoCapture cam;
	cv::Mat vidFrame, lastFrame;
	int pid = 0;


	cam.open(0);
	if (!cam.isOpened())
		throw "failed to open camera";

	pid = getpid();
	if (setpriority(PRIO_PROCESS, pid, 20) == -1 && errno != 0)
		throw strerror(errno);

	while (true)
	{
		cam >> vidFrame;
		if (cv::norm(vidFrame) == cv::norm(lastFrame))
			continue;
		vidFrame.copyTo(lastFrame);

		std::cout << "Norm of image matrix: " << cv::norm(vidFrame) << std::endl;
		REPOSITION_CURSOR_LAST_LINE;
	}
	return 0;
}
