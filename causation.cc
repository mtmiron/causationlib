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

#define SHOW_IMAGES 1

#define RING_SIZE 5
#define REPOSITION_CURSOR_LAST_LINE std::cout << '\x08' << '\x0D'

static const char* visual_channel_map[] = {
	"CAP_OPENNI_DEPTH_MAP        ",
	"CAP_OPENNI_POINT_CLOUD_MAP  ",
	"CAP_OPENNI_DISPARITY_MAP    ",
	"CAP_OPENNI_DISPARITY_MAP_32F",
	"CAP_OPENNI_VALID_DEPTH_MASK ",
	"CAP_OPENNI_BGR_IMAGE        ",
	"CAP_OPENNI_GRAY_IMAGE       ",
//	"CAP_OPENNI_IR_IMAGE         ",
	NULL,
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
	static const float DELTA_THRESHOLD = 0.10;

	static cv::VideoCapture cam(cv::CAP_OPENNI);
	static std::vector<cv::Mat> vidFrame;

	// ring buffers for tracking changes over multiple calls
	static std::vector<float> norm[RING_SIZE], delta[RING_SIZE];
	static int ringidx = 0;

	// Index of the last iteration's ring buffer ptr
	int lringidx = (ringidx == 0 ? RING_SIZE - 1 : ringidx - 1);

	bool event_took_place = false;

	if (!cam.isOpened())
		throw "failed to open camera";

	cam.grab();
	for (int i = 0; visual_channel_map[i] != NULL; i++)
	{
		// Make sure the framebuffer has allocated a Mat for each channel
		if (vidFrame.size() < i+1) {
			vidFrame.push_back(cv::Mat());
		}

		// Make sure the ring buffer ptrs have allocated a slot for each channel
		if (norm[ringidx].size() < vidFrame.size()) {
			norm[ringidx].push_back(0.0);
			delta[ringidx].push_back(0.0);
		}

		// Retrieve the frame from channel i and save its matrix norm
		cam.retrieve(vidFrame[i], i);
		norm[ringidx][i] = cv::norm(vidFrame[i]);
	}

	// Print out the values and calculate the deltas of the norms between last call and this one
	for (int i = 0; i < vidFrame.size(); i++) {
		if (norm[lringidx].size() > 0)
			delta[ringidx][i] = abs(norm[ringidx][i] - norm[lringidx][i]);
		std::cout << visual_channel_map[i] << "\t" << norm[ringidx][i] << " \t " << delta[ringidx][i] << std::endl;
	}

	// Check for sufficiently large variations throughout entire ring buffer
	for (int i = 0; i < vidFrame.size(); i++) {
		/*
		// Check threshold against weighted value of each tracked delta and this iteration's delta
		for (int j = 0; j < RING_SIZE && delta[j].size() > 0; j++)
			if ( (norm[ringidx][i] * DELTA_THRESHOLD) <= ((delta[j][i] + delta[ringidx][i]) / 1.5) ) {
				event_took_place = !clock_gettime(CLOCK_REALTIME, &event);
				if (SHOW_IMAGES)
					imshow(visual_channel_map[i], vidFrame[i]);
			}
		*/
		// check threshold against this iteration's delta alone
		if ( (norm[ringidx][i] * DELTA_THRESHOLD) <= delta[ringidx][i] ) {
			event_took_place = !clock_gettime(CLOCK_REALTIME, &event);
			if (SHOW_IMAGES)
				imshow(visual_channel_map[i], vidFrame[i]);
		}
		REPOSITION_CURSOR_LAST_LINE;
	}

	// HighGUI needs this to repaint the image in the window
	if (SHOW_IMAGES)
		cv::waitKey(1);

	if (++ringidx >= RING_SIZE)
		ringidx = 0;

//	if (event_took_place)
//		for (int i = 0; i < RING_SIZE && delta[i].size() > 0; i++)
//			delta[i].clear();
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
//		usleep(40000);
	}
	return 0;
}
