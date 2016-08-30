# causationlib
Personal backup of some theories in a uselessly-early state (i.e. nothing to see here, move on)

This is a collection of simulations and implementations of theoretical frameworks.  Usefulness is neither guaranteed nor provided; a deeper understanding on the part of the author is, however, selfishly maximized with no concern whatsoever for any other parameters.

Note that the TimeQueue functions from the tortoise library are not generic -- they're designed to function with the neural net code, and it will not postpone events until the arbitrary time provided: it perfectly preserves the chain of causality through time, but it does not postpone execution of events until a given time.

What that statement is actually saying: so long as "events," are only handled by the TimeQueue functions, and you don't have a separate thread off doing things that haven't been inserted into the queue, then your CPU doesn't have to keep up and the simulation can be as demanding as necessary.  If, however, you want something to execute at time T+10, the TimeQueue must be handling it, or you run the risk of the TimeQueue executing, say, something at T+15 first because it wasn't aware of the T+10 event (which, obviously, breaks causality and the entire simulation is void.)
