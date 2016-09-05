# causationlib
Personal backup of some theories and simulations; real world applications are unlikely to be found here.

This is a collection of simulations and implementations of theoretical frameworks.  Usefulness is neither guaranteed nor provided; a deeper understanding on the part of the author is, however, selfishly maximized with no concern whatsoever for any other parameters.

The neural net code is designed to mimic very simple neurophysiology, not implement established AI algorithms; it ignores many forces that influence real biology, but since the entire point of this project is to see what happens when the temporal dimension of reality is thrown in to the mix (i.e. simple 4D spacetime instead of 3D Euclidean space), it's good enough.

Note that the TimeQueue functions from the tortoise library are not generic -- they're designed to function with the neural net code, and it will not postpone events until the arbitrary time provided: it perfectly preserves the chain of causality through time, but it does not postpone execution of events until a given time.  It's a bare-bones design that takes about as many words to describe as it does lines of code to implement.

What that statement is actually saying: so long as "events," are only handled by the TimeQueue functions, and you don't have a separate thread off doing things that haven't been inserted into the queue, then your CPU doesn't have to keep up and the simulation can be as demanding as necessary.  If, however, you want something to execute at time T+10, the TimeQueue must be handling it, or you run the risk of the TimeQueue executing, say, something at T+15 first because it wasn't aware of the T+10 event (which, obviously, breaks causality and the entire simulation is void.)


### Required
If for some reason you want to run the code here, you'll need:
 - CMake
 - OpenCV
 - g++ or an equivalent C++ compiler
