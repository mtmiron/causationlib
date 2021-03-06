#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <opencv2/highgui.hpp>
#include <time.h>
#include <math.h>
#include "temporalnn.hh"

#define REPOSITION_CURSOR_LAST_LINE std::cout << '\x08' << '\x0D' \
<< "                                                                      "\
<< '\x0D'


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
static const char* visual_channel_map[] = {
#ifdef BUILD_WITH_OPENNI
#  define NUMVCHANNELS 7
#  define EVENT_TRIGGER_PERCENT 0.10
	"CAP_OPENNI_DEPTH_MAP        ",
	"CAP_OPENNI_POINT_CLOUD_MAP  ",
	"CAP_OPENNI_DISPARITY_MAP    ",
	"CAP_OPENNI_DISPARITY_MAP_32F",
	"CAP_OPENNI_VALID_DEPTH_MASK ",
	"CAP_OPENNI_BGR_IMAGE        ",
	"CAP_OPENNI_GRAY_IMAGE       ",
	"CAP_OPENNI_IR_IMAGE         ",
#else
#  define NUMVCHANNELS 1
#  define EVENT_TRIGGER_PERCENT 0.03
	"MISC_VIDEO_CHANNEL          ",
#endif
};

/*
 * Channels to do actual processing on
 */
static const int processed_visual_channels[] = {
#ifdef BUILD_WITH_OPENNI
	5, 6,
#else
	0,
#endif
};

/*
 * Command line options
 */
static struct options {
	bool show_images;
	bool show_visual_norms;
} options;



/*
 * Do computationally expensive processing on an image frame
 */
void visual_processing(cv::Mat &frame)
{

}



/*
 * Rudimentary visual change detection:
 *
 * An image frame is a matrix, each element corresponding to an
 * intensity value at that pixel.  The norm of a matrix is
 * the equivalent of the magnitude of that matrix; thus,
 * the norm of an image frame is a number representing the
 * intensity of the pixels.
 *
 * With a Kinect making depth/IR values available, this works
 * surprisingly well for almost no computational cost.
 *
 * Since a "visual event," is primarily meant to trigger
 * re-evaluation of a scene, false positives are acceptable.
 */
int visual_iter(struct timespec &event)
{
#ifdef BUILD_WITH_OPENNI
	static cv::VideoCapture cam(cv::CAP_OPENNI);
#else
	static cv::VideoCapture cam(0);
#endif
	static std::vector<cv::Mat> vidFrame(NUMVCHANNELS);
	static std::vector<float> old_norm(NUMVCHANNELS), cur_norm(NUMVCHANNELS), delta(NUMVCHANNELS);
	int event_took_place = -1;

	if (!cam.isOpened())
	{
		cerr << "unable to open camera" << endl;
		abort();
	}

	cam.grab();
	for (int i = 0; i < NUMVCHANNELS; i++)
	{
		cam.retrieve(vidFrame[i], i);
		cur_norm[i] = cv::norm(vidFrame[i]);
	}

	// Calculate change in matrix magnitude for each video channel
	for (uint i = 0; i < vidFrame.size(); i++) {
		delta[i] = abs(cur_norm[i] - old_norm[i]);
		if (options.show_visual_norms)
			std::cout << visual_channel_map[i] << "\t" << cur_norm[i] << " \t " << delta[i] << std::endl;
	}

	// Track any sudden changes in our entire field of vision (from last call to this one)
	for (uint i = 0; i < vidFrame.size(); i++) {
		old_norm[i] = cur_norm[i];
		if ( (cur_norm[i] * EVENT_TRIGGER_PERCENT) <= delta[i] ) {
			event_took_place = i;
			clock_gettime(CLOCK_REALTIME, &event);
		}
		if (options.show_visual_norms)
			REPOSITION_CURSOR_LAST_LINE;
	}

	// Do extra processing on visual channels of interest
	for (uint i = 0; i < (sizeof(processed_visual_channels) / sizeof(*processed_visual_channels)); i++)
		visual_processing(vidFrame[processed_visual_channels[i]]);

	if (options.show_images) {
		for (int i = 0; i < NUMVCHANNELS; i++)
			imshow(visual_channel_map[i], vidFrame[i]);
		cv::waitKey(1);
	}

	return event_took_place;
}

void print_help(char **argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n"
		"\t\t-h\tThis help\n"
		"\t\t-s\tShow images\n"
		"\t\t-n\tShow norm of image frame matrices\n",
		*argv);
	exit(0);
}

struct options parse_args(int argc, char **argv)
{
	int c = 0;

	while ((c = getopt(argc, argv, "shn")) != -1)
	{
		switch (c) {
		case 's':
			options.show_images = true;
			break;
		case 'h':
			print_help(argv);
			break;
		case 'n':
			options.show_visual_norms = true;
			break;
		}
	}
	return options;
}

int main(int argc, char **argv)
{
	int pid = 0;
	int visual_event_on_channel = -1;
	struct timespec event_times[1] = { 0 };

	parse_args(argc, argv);

	pid = getpid();
	if (setpriority(PRIO_PROCESS, pid, 20) == -1 && errno != 0) {
		std::cerr << strerror(errno) << endl;
	}

	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(5);
	if (options.show_visual_norms)
		std::cout << "Norm of image matrices: " << std::endl << std::endl;

	while (true)
	{
		if ( (visual_event_on_channel = visual_iter(event_times[0])) != -1) {
			std::cout << "visual event on channel " << visual_event_on_channel
				<< " at " << event_times[0].tv_sec << "." << event_times[0].tv_nsec << std::endl;
			if (options.show_visual_norms) {
				std::cout << std::endl;
				REPOSITION_CURSOR_LAST_LINE;
			}
		}
	}
	return 0;
}
