#include <unistd.h>
#include <stdio.h>
#include "temporalnn.hh"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#ifdef BUILD_WITH_MULTITHREADING
#	include <thread>
#endif

#define DEFAULT_STEP_SIZE 95
#define DEFAULT_NET_HEIGHT 360
#define DEFAULT_NET_WIDTH 480
#define DEFAULT_LOOP_TIME 10
#define DEFAULT_INPUT_STRENGTH 50
#define DEFAULT_LAYERS 1
#define DEFAULT_FADE_TIME 1
#define DEFAULT_RANDOM_FIRING true

using namespace std;
using namespace cv;


struct options {
	int stepsize = DEFAULT_STEP_SIZE;
	int height = DEFAULT_NET_HEIGHT;
	int width = DEFAULT_NET_WIDTH;
	int loop_time = DEFAULT_LOOP_TIME;
	int input_strength = DEFAULT_INPUT_STRENGTH;
	uint layers = DEFAULT_LAYERS;
	float fade_time = DEFAULT_FADE_TIME;
	int propagation_time = -1;
	int max_dendrite_bulge = -1;
	int excited_time = -1;
	int refractory_time = -1;
	bool random_step = DEFAULT_RANDOM_FIRING;
	bool input_enabled = true;
	bool freeze_connections = false;
	bool no_density_image = false;
	bool no_activity_image = false;
	bool camera_input = false;
	bool debug = false;
	bool draw_weakly_stimulated = false;
};

struct options opts;
static TortoiseTime last_loop_time;
extern TimeQueue BrainCell::event_queue;
extern bool BrainCell::freeze_connections;
extern bool BrainCell::shrink_dendrites;
extern bool BrainCell::debug;


void print_help(char *argv)
{
	printf("Usage: %s [opts]\n\n\tOptions are:\n"
		"\t\t-h\tThis help\n"
//		"\t\t-D\tDump huge amounts of debug data\n"
		"\t\t-d\tDon't draw neural connection density image\n"
		"\t\t-a\tDon't draw neural activity image\n"
		"\t\t-R\tDon't randomize step size for neuron input loop\n"
		"\t\t-F\tDon't self assemble neurons (freeze state)\n"
		"\t\t-c\tGrab frames from a camera and use the pixels as input\n"
		"\t\t-S\tShrink dendrites, as well as grow them\n"
		"\t\t-w\tDraw neuron activity even when too weakly stimulated to fire\n"
		"\t\t-p ARG\tPropagation time (time from neuron firing to hitting next neuron)\n"
		"\t\t-f ARG\tFade time for activity image\n"
		"\t\t-s ARG\tStep size for neuron input loop\n"
		"\t\t-x ARG\tWidth of neural net\n"
		"\t\t-y ARG\tHeight of neural net\n"
		"\t\t-i ARG\tSet the input strength to each neuron\n"
		"\t\t-t ARG\tSet the wait time between wave-input iterations\n"
		"\t\t-l ARG\tNumber of neural nets\n"
		"\t\t-m ARG\tMax dendritic bulge (directly related to max connections)\n"
		"\t\t-e ARG\tSet excited_time ('concurrent' input within this time will sum)\n"
		"\t\t-r ARG\tSet neuron refractory time (time period until they'll fire again)\n"
		"\n", argv);
	exit(0);
}


void print_status()
{
	fprintf(stdout, "\nInput: %dmV  Step size: %d%%  Net height: %d  Net width: %d\n"
			"Firing wave: %dms  Net layers: %d  Fade time: %f  Propagation time: %dms  Refractory time: %dms\n"
			"Max dendrite bulge: %d  Excited duration: %dms\n\n"

			"Images are updated once per firing wave (%dms); red means firing, blue means sub-action-potential stimulation, green is firing triggered by concurrent weak stimulation\n\n"

			"Keymap:\n"
			"ESC - exit\n"
			"'h' - print this information\n"
			"'p' - toggle pause\n"
			"'r' - toggle random firing\n"
			"'d' - toggle updating of connection densities window(s)\n"
			"'a' - toggle updating of firing neurons window(s)\n"
			"'i' - toggle input loop\n"
			"'c' - clear the event queue\n"
			"'f' - freeze connections\n"
			"'-' - decrease step size\n"
			"'+' - increase step size\n\n"
			"Mouse LButton - fire neurons at mouse (x,y) position\n"
			"Mouse RButton - call NeuralNet's growDendrite() function\n"
			"Mouse MButton - dump info about a neuron under the cursor\n",

			opts.input_strength, opts.stepsize, opts.height, opts.width,
			opts.loop_time, opts.layers, opts.fade_time, opts.propagation_time,
			opts.refractory_time, opts.max_dendrite_bulge, opts.excited_time,
			opts.loop_time);
}


struct options parse_args(int argc, char **argv)
{
	int c = 0;

	while ((c = getopt(argc, argv, "x:y:hs:dai:l:t:Rr:Ff:p:m:e:cDSw")) != -1)
	{
		switch (c) {
		case 'c':
			opts.camera_input = true;
			opts.input_enabled = false;
			opts.max_dendrite_bulge = 2;
#ifdef BUILD_WITH_MULTITHREADING
			opts.fade_time = 1.0;
#else
			opts.fade_time = 0.0;
#endif
			break;
		case 'S':
			BrainCell::shrink_dendrites = true;
			break;
		case 'w':
			opts.draw_weakly_stimulated = true;
			break;
		case 'D':
			opts.debug = true;
			BrainCell::debug = true;
			break;
		case 's':
			opts.stepsize = atoi(optarg);
			break;
		case 'x':
			opts.width = atoi(optarg);
			break;
		case 'y':
			opts.height = atoi(optarg);
			break;
		case 'a':
			opts.no_activity_image = true;
			break;
		case 'd':
			opts.no_density_image = true;
			break;
		case 'i':
			opts.input_strength = atoi(optarg);
			break;
		case 't':
			opts.loop_time = atoi(optarg);
			break;
		case 'l':
			opts.layers = atoi(optarg);
			break;
		case 'R':
			opts.random_step = !opts.random_step;
			break;
		case 'r':
			opts.refractory_time = atoi(optarg);
			break;
		case 'p':
			opts.propagation_time = atoi(optarg);
			break;
		case 'f':
			opts.fade_time = atof(optarg);
			break;
		case 'F':
			opts.freeze_connections = true;
			BrainCell::freeze_connections = true;
			break;
		case 'm':
			opts.max_dendrite_bulge = atoi(optarg);
			break;
		case 'e':
			opts.excited_time = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			print_help(*argv);
		}
	}
	return opts;
}


void handle_keypress(uchar key)
{
	switch (key & 0xff) {
	case 'p':
		while ( ((key = waitKey(0)) & 0xff) != 'p')
			if (key == 27)
				exit(0);
		break;
	case 'c':
		cout << "Cleared " << BrainCell::event_queue.clear() << " pending action potential and/or assembly events" << endl;
		break;
	case 'f':
		BrainCell::freeze_connections = !BrainCell::freeze_connections;
		cout << "Neuron connections " << (BrainCell::freeze_connections ? "frozen" : "unfrozen") << endl;
		break;
	case 'r':
		opts.random_step = !opts.random_step;
		cout << "Random stimulation " << (opts.random_step ? "enabled" : "disabled") << endl;
		break;
	case 'a':
		opts.no_activity_image = !opts.no_activity_image;
		cout << "Repainting activity image " << (opts.no_activity_image ? "disabled" : "enabled") << endl;
		break;
	case 'd':
		opts.no_density_image = !opts.no_density_image;
		cout << "Repainting density image " << (opts.no_density_image ? "disabled" : "enabled") << endl;
		break;
	case 'h':
		print_status();
		break;
	case 'i':
		opts.input_enabled = !opts.input_enabled;
		break;
	case '-':
	case '_':
	case 173:
		if (opts.stepsize > 1)
			opts.stepsize--;
		cout << "Step size: " << opts.stepsize << "%" << endl;
		break;
	case '+':
	case '=':
	case 171:
		if (opts.stepsize < 99)
			opts.stepsize++;
		cout << "Step size: " << opts.stepsize << "%" << endl;
		break;
	}
}


void activity_window_mouse_callback(int event, int x, int y, int flags, void *userdata)
{
	Neuron *neuron = NULL;
	NeuralNet *net = (NeuralNet *)userdata;
	static bool rbutton_down = false, lbutton_down = false, mbutton_down = false;
			
  try {
	switch (event) {
	case EVENT_RBUTTONDOWN: rbutton:
		rbutton_down = true;
		neuron = &net->getFromWindowPosition(x, y, opts.width, opts.height);
		net->growDendrite(neuron, neuron->numberOfConnections() + 2);
		break;
	case EVENT_LBUTTONDOWN: lbutton:
		lbutton_down = true;
		neuron = &net->getFromWindowPosition(x, y, opts.width, opts.height);
		neuron->input(opts.input_strength, last_loop_time + opts.loop_time);
		break;
	case EVENT_MBUTTONDOWN: mbutton:
		mbutton_down = true;
		neuron = &net->getFromWindowPosition(x, y, opts.width, opts.height);
		cout << *neuron << endl;
		break;
	case EVENT_MOUSEMOVE:
		if (lbutton_down) goto lbutton;
		else if (rbutton_down) goto rbutton;
		else if (mbutton_down) goto mbutton;
		break;
	case EVENT_LBUTTONUP:
		lbutton_down = false;
		break;
	case EVENT_RBUTTONUP:
		rbutton_down = false;
		break;
	case EVENT_MBUTTONUP:
		mbutton_down = false;
		break;
	}
  } catch (NeuralNetException &e) { cerr << "bad (x,y): (" << x << "," << y << ")" << endl; }
}


void density_window_mouse_callback(int event, int x, int y, int flags, void *userdata)
{
	activity_window_mouse_callback(event, x, y, flags, userdata);
}


void interval_step(vector<NeuralNet *> &nets, TortoiseTime at_time)
{
	uint step_x = nets[0]->neurons.size() / (100 - opts.stepsize);
	uint step_y = nets[0]->neurons[0].size() / (100 - opts.stepsize);

	for (uint i = 0; i < nets[0]->neurons.size(); i += step_x)
		for (uint j = 0; j < nets[0]->neurons[i].size(); j += step_y)
			nets[0]->neurons[i][j].input(opts.input_strength, at_time);
}


void random_step(vector<NeuralNet *> &nets, TortoiseTime at_time)
{
	uint step_x = nets[0]->neurons.size() / (100 - opts.stepsize);
	uint step_y = nets[0]->neurons[0].size() / (100 - opts.stepsize);

	for (uint i = (random() % step_x); i < nets[0]->neurons.size(); i += (random() % step_x))
		for (uint j = (random() % step_y); j < nets[0]->neurons[i].size(); j += (random() % step_y))
			nets[0]->neurons[i][j].input(opts.input_strength, at_time);
}


void frame_step(vector<NeuralNet *> &nets, TortoiseTime at_time, Mat &frame)
{
	Size dim = frame.size();
	uchar pixel = 0;
	Neuron *n;

	for (uint i = 0; i < dim.width; i++)
		for (uint j = 0; j < dim.height; j++) {
			pixel = frame.at<uchar>(j, i);
			try {
				n = &nets[0]->getFromWindowPosition(i, j, dim.width, dim.height);
				n->input(pixel, at_time);
			} catch (...) { }
		}
}


/*
 * NOTE:
 *
 * no way to specify the time at which to grab a frame, so
 * this function dictates the time of neuron input as the time
 * the frame was grabbed and returns it for the main loop.
 */
TortoiseTime camera_window_update(vector<NeuralNet *> &nets, VideoCapture &cam)
{
	Mat frame, color_frame;
	TortoiseTime at_time;

#ifdef BUILD_WITH_OPENNI
	cam.retrieve(frame, CAP_OPENNI_GRAY_IMAGE);
#else
	cam.retrieve(color_frame, 0);
	cvtColor(color_frame, frame, CV_BGR2GRAY);
#endif

	imshow("camera frame", frame);
	clock_gettime(CLOCK_REALTIME, &at_time);
	frame_step(nets, at_time, frame);

	return at_time;
}


void activity_window_update(vector<NeuralNet *> &nets, TortoiseTime at_time)
{
	static char windowname[256] = { 0 };
	static bool set_callback = true;
	Mat activity_image;

	for (uint i = 0; i < opts.layers; i++)
	{
		sprintf(windowname, "net (layer) #%d: neuron activity", i);
		activity_image = nets[i]->createCurrentActivityImage(opts.width, opts.height, at_time, opts.fade_time, opts.draw_weakly_stimulated);
		imshow(windowname, activity_image);
		if (set_callback)
			setMouseCallback(windowname, activity_window_mouse_callback, nets[i]);
	}
	set_callback = false;
}


void density_window_update(vector<NeuralNet *> &nets, TortoiseTime at_time)
{
	static char windowname[256] = { 0 };
	static bool set_callback = true;
	Mat densities_image;

	for (uint i = 0; i < opts.layers; i++)
	{
		sprintf(windowname, "net (layer) #%d: neuron connection densities", i);
		densities_image = nets[i]->createConnectionDensityImage(opts.width, opts.height);
		imshow(windowname, densities_image);
		if (set_callback)
			setMouseCallback(windowname, density_window_mouse_callback, nets[i]);
	}
	set_callback = false;
}


int main(int argc, char **argv)
{
	vector<NeuralNet *> nets;
	TortoiseTime at_time;
	VideoCapture cam;
	try {
#ifdef BUILD_WITH_OPENNI
		cam = VideoCapture(cv::CAP_OPENNI);
#else
		cam = VideoCapture(0);
#endif
	} catch (...) { }

	opts = parse_args(argc, argv);
	clock_gettime(CLOCK_REALTIME, &at_time);
	srandom((int)at_time.tv_sec);

	for (uint i = 0; i < opts.layers; i++)
	{
		nets.push_back(new NeuralNet(opts.width, opts.height));
		for (int x = 0; x < opts.width; x++)
			for (int y = 0; y < opts.height; y++)
			{
				if (opts.propagation_time != -1)
					nets[i]->neurons[x][y].setPropagationTime(opts.propagation_time);
				else
					opts.propagation_time = nets[i]->neurons[x][y].propagation_time;
				if (opts.max_dendrite_bulge != -1)
					nets[i]->neurons[x][y].setMaxDendriteConnections(opts.max_dendrite_bulge);
				else
					opts.max_dendrite_bulge = nets[i]->neurons[x][y].max_dendrite_bulge;
				if (opts.excited_time != -1)
					nets[i]->neurons[x][y].setExcitedTime(opts.excited_time);
				else
					opts.excited_time = nets[i]->neurons[x][y].excited_time;
				if (opts.refractory_time != -1)
					nets[i]->neurons[x][y].setRefractoryTime(opts.refractory_time);
				else
					opts.refractory_time = nets[i]->neurons[x][y].refractory_time;
			}
	}
	print_status();

	for (uint i = 0; i < opts.layers - 1; i++) {
		nets[i]->connectTo(nets[i+1]);
		nets[i+1]->connectTo(nets[i]);
	}

  try {
	// out of memory buffer, so the catch() has some when it goes out of scope
	volatile char oom_buf[32 * 1024];
#ifdef BUILD_WITH_MULTITHREADING
	thread thr;
#endif

	for (uchar key = 0; key != 27; key = waitKey(opts.loop_time))
	{
		handle_keypress(key);

		if (opts.camera_input) {
			cam.grab();
			at_time = camera_window_update(nets, cam);
		} else {
			clock_gettime(CLOCK_REALTIME, &at_time);
		}

		last_loop_time = at_time;

		if (opts.input_enabled) {
			if (opts.random_step)
				random_step(nets, at_time);
			else
				interval_step(nets, at_time);
		}

#ifdef BUILD_WITH_TIMEQUEUE
#  ifdef BUILD_WITH_MULTITHREADING
		if (!thr.joinable())
			thr = thread(&TimeQueue::applyAllUpto, &BrainCell::event_queue, at_time);
		else
			thr.join();
#  else
		BrainCell::event_queue.applyAllUpto(at_time);
#  endif
#endif
		if (!opts.no_activity_image)
			activity_window_update(nets, at_time);
		if (!opts.no_density_image)
			density_window_update(nets, at_time);
	}

  } catch (bad_alloc &memerr) {
		cerr << "allocation error: " << memerr.what() << endl;
		abort();
  }
	return 0;
}
