#include "temporalnn.hh"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace TemporalNet;
using namespace cv;


/*
 * Construct a cv::Mat (image frame) from a neural net.  Must be <= 2 dimensions.
 *
 * The resulting image is a rough representation with connected clusters indicated by
 * brighter pixel values; no visual distinctions between individual neurons is shown.
 */
Mat createImageFromNet(NeuralNet &net, int width = 1024, int height = 768);
