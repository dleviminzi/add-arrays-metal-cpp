# GPU Array Addition Using Metal

I followed the tutorial [here](https://developer.apple.com/documentation/metal/performing_calculations_on_a_gpu) for performing calculations on the GPU using Metal.
I chose to rewrite the example using C++ and the metal-cpp library. The metal-cmake setup can be found in [this repository](https://github.com/LeeTeng2001/metal-cpp-cmake) along with other helpful examples.

The goal was to learn about how GPU computation pipelines work. In summary: 
1. Request a GPU device.
2. Establish a library on the device that consists of compiled MSL.
3. Use functions to create pipeline state.
4. Get a command queue, command buffer, and command encoder.
5. Use the encoder to load variables, pipeline state, and thread specification.
6. Close the encoder and commit the buffer to the queue.
7. Wait for your work to be scheduled and executed.