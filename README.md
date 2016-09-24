# causationlib
This is a collection of simulations and implementations of theoretical frameworks.  Usefulness is neither guaranteed nor provided; a deeper understanding on the part of the author is, however, selfishly maximized with no concern whatsoever for any other parameters.

The neural net code is designed to mimic very simple neurophysiology, not implement established AI algorithms; it ignores many forces that influence real biology, but since the entire point of this project is to see what happens when the temporal dimension of reality is preserved properly, it's good enough.  The complexity of modeling neurotransmitter synapses is not currently a priority: for now it's all based on membrane voltages and ensuring that neuron action potentials (what they call it when neurons fire) propagate and combine as input to individual neurons precisely as they would in a real nervous system (as opposed to a more efficient design which wouldn't preserve the effects of concurrent sub-action-potential [weak] input to a single neuron.)


### Required
If for some reason you want to run the code here, you'll need:
 - CMake
 - OpenCV
 - g++ or an equivalent C++ compiler

If you want to grab image frames from a webcam and use that as input to a neural net (using the netpeek executable), you'll also need:
 - any attached camera with drivers OpenCV can utilize
