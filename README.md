# causationlib
This is a collection of simulations and implementations of theoretical frameworks.  Usefulness is neither guaranteed nor provided; a deeper understanding on the part of the author is, however, selfishly maximized with no concern whatsoever for any other parameters.

The neural net code is designed to mimic very simple neurophysiology, not implement established AI algorithms; it ignores many forces that influence real biology, but since the entire point of this project is to see what happens when the temporal dimension of reality is preserved properly, it's good enough.  The complexity of modeling neurotransmitter synapses is not currently a priority: for now it's all based on membrane voltages and ensuring that neuron action potentials (what they call it when neurons fire) propagate and combine as input to individual neurons precisely as they would in a real nervous system (as opposed to a more efficient design which wouldn't preserve the effects of concurrent sub-action-potential [weak] input to a single neuron.)

That statement should probably be qualified by saying that manipulating the parameters of neurophysiology on-the-fly to see what changes, is what's meant by "precisely as they would in a real nervous system"  :)


### What's with the name?
Cause and effect: I've been pondering, of late, what the foundation of human intelligence is (from a more psychological perspective than anything.)  My feeling is that an incredibly useful abstraction of the bulk of human intellect could be obtained if a way to judge what caused a certain event to happen could be found -- something along the lines of a program learning how to recreate observed events, which would allow very simple and very standard pathfinding algorithms to, say, look at a room and figure out how to manipulate the objects into a goal state.

So I'm playing with causation, and the pieces are a library.  And that's about it.


### Required
If for some reason you want to run the code here, you'll need:
 - CMake
 - OpenCV
 - g++ or an equivalent C++ compiler

### Optional
If you want to grab image frames from a webcam and use that as input to a neural net (using the _netpeek_ executable), you'll also need:
 - any attached camera with drivers OpenCV can utilize

If you have difficulty with the neuron activity windows when running the _netpeek_ executable, the issue is probably the fade time: use the _-f_ argument to set the amount of prior activity that's displayed (in seconds.)


