//
// Created by Daniel Levi-Minzi on 8/24/22.
//

#include "MetalAdder.h"
#include <iostream>

const int bufferSize = 5;
const int arrayLength = bufferSize * sizeof(float);

MetalAdder::MetalAdder(MTL::Device *device) {
    NS::Error* error = nil;

    _mDevice = device;
    if (_mDevice == nil) {
        throw std::runtime_error("failed to initialize device");
    }

    // establish the compiled "add" metal library on the device
    NS::String* filePath = NS::String::string("/Users/minzi/Dev/Metal/00/metal-lib/add.metallib", NS::UTF8StringEncoding);
    MTL::Library* library = _mDevice->newLibrary(filePath, &error);
    if (library == nil) {
        throw std::runtime_error("failed to initialize default library");
    }

    // create a reference to our msl "add_arrays" function from our library
    MTL::Function* addFunction = library->newFunction(reinterpret_cast<const NS::String *>("add_arrays"));
    if (addFunction == nil) {
        throw std::runtime_error("failed to initialize add function");
    }

    // create compute pipeline for our msl "add_arrays" function (actual executable call to our function)
    _mAddFunctionPSO = _mDevice->newComputePipelineState(addFunction, &error);
    if (_mAddFunctionPSO == nil) {
        throw std::runtime_error("failed to initialize add function PSO");
    }

    // get command queue from device to send it work
    _mCommandQueue = _mDevice->newCommandQueue();
    if (_mCommandQueue == nil) {
        throw std::runtime_error("failed to initialize command queue");
    }

    // create data buffers on the GPU and load some random data into them (CPU and GPU can write to this space given the
    //                                                                     shared mode option)
    _mBufferA = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mBufferB = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mBufferResult = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
}

// PrepareData loads random floats into the allocated device buffers
void MetalAdder::PrepareData() {
    float buff1[arrayLength];
    float buff2[arrayLength];

    for (int i = 0; i < arrayLength; i++) {
        buff1[i] = float(rand())/float(RAND_MAX);
        buff2[i] = float(rand())/float(RAND_MAX);
    }

    // Metal buffers are allocations of memory with no predefined format. It is simply a collection of bytes.
    // The app and the shader must agree on the format of the data being passed back and forth. In this case,
    // the msl we wrote specifies the addition of floats so we are copying floats into our buffers.
    memcpy(_mBufferA->contents(), buff1, arrayLength);
    memcpy(_mBufferB->contents(), buff2, arrayLength);
}

void MetalAdder::SendComputeCommand() {
    // Create a command buffer
    auto commandBuffer = _mCommandQueue->commandBuffer();
    if (commandBuffer == nil) {
        throw std::runtime_error("failed to initialize command buffer");
    }

    // Create a command encoder for the specific type of command we are using: compute
    // Each compute command causes the GPU to create a grid of threads to execute on it.
    auto computeEncoder = commandBuffer->computeCommandEncoder();
    if (computeEncoder == nil) {
        throw std::runtime_error("failed to initialize compute encoder");
    }

    // Next we make calls to set things like the state and the arguments into the command
    computeEncoder->setComputePipelineState(_mAddFunctionPSO);
    computeEncoder->setBuffer(_mBufferA, 0, 0); /* indices follow the order in the MSL we wrote */
    computeEncoder->setBuffer(_mBufferB, 0, 1);
    computeEncoder->setBuffer(_mBufferResult, 0, 2);

    // Decide how many threads to use and what layout to use them in. In this case, 1 dimension of array length.
    MTL::Size gridSize = MTL::Size(arrayLength, 1, 1);

    // Metal will divide the grid into thread groups that can be executed by different parts of the GPU to speed things up.
    // Here we set the group size to be the array length if the max is greater than it.
    NS::UInteger threadGroupSize = _mAddFunctionPSO->maxTotalThreadsPerThreadgroup();
    if (threadGroupSize > arrayLength) {
        threadGroupSize = arrayLength;
    }

    MTL::Size tGSize = MTL::Size(threadGroupSize, 1, 1);


    // Dispatch threads is the call that starts the GPU processing the encoded computation
    computeEncoder->dispatchThreads(gridSize, tGSize);

    // Now we are finished with encoding and can commit the buffer to the command queue where it will be scheduled
    // and then executed. We wait until it is done executing to exit this scope.
    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

// Simple helper function to check that the addition was performed correctly.
void MetalAdder::Verify() {
    float buffA[arrayLength];
    float buffB[arrayLength];
    float buffResult[arrayLength];

    memcpy(buffA, _mBufferA->contents(), arrayLength);
    memcpy(buffB, _mBufferB->contents(), arrayLength);
    memcpy(buffResult, _mBufferResult->contents(), arrayLength);

    for (int i = 0; i < _mBufferA->length(); i++) {
        std::cout << "buffer A: " << buffA[i] << " buffer B: " << buffB[i] << " buffers added: " << buffResult[i] << std::endl;
        std::cout << "buffer A + buffer B = buffers added? " << (buffA[i] + buffB[i] == buffResult[i]) << std::endl;
    }
}
