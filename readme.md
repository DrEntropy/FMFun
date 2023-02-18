# FM Fun

* This is intended to be a very simple FM synth , probably will be simply 2 op with feedback fm  and a simple ladder filter on the output. It is intended as a learning project to explore synthesizers with JUCE.

* see the two block diagrams

* As fo Feb 18 2023, I am mothballing this project. As it is it is a very barebones 2 op fm synth.  

   - Each Op has:
     - Feedback modulation
     - Envelope Generator 
    
   - Overall there is a ladder filter (with EG for cuttoff) as well pitch modulation.

   - only two algorithms are available: Parallel and sequential. 

* Future work that could be done:

[ ]  Implement Operator Classes:
     [ ]  Create parameter helpers to create id strings
     [ ]  Factor all operator controls OperatorControl (placeholder file)
     [ ]  Factor all operator processing into seperate classes.
[ ]  Add LFO
[ ]  Add Key Scaling of Mod and Feedback
[ ]  Add fixed frequency option for carrier
[ ]  Test all parameters and fix ranges and step size
[ ]  Improve Efficiency / bug fix
    [ ]  Move pitch env out of processing loop
    [ ]  Amplitude attack is no quite right
