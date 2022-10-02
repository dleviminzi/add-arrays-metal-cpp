//
// Created by Daniel Levi-Minzi on 8/24/22.
//

#include "MetalAdder.h"
#define MTL_PRIVATE_IMPLEMENTATION

#include <iostream>

const int bufferSize = 5;
const int arrayLength = bufferSize * sizeof(float);

MetalAdder::MetalAdder(MTL::Device *device) {
    NS::Error* error = nil;

    _mDevice = device;
    if (_mDevice == nil) {
        throw std::runtime_error("failed to initialize device");
    }

    _mCommandQueue = _mDevice->newCommandQueue();
    if (_mCommandQueue == nil) {
        throw std::runtime_error("failed to initialize command queue");
    }

    // currently this method will return nullptr. since I'm not using xcode, I will need to compile
    // and then use newLibrary(path, etc)

    NS::String* filePath = NS::String::string("/Users/minzi/Dev/Metal/00/metal-lib/add.metallib", NS::UTF8StringEncoding);
    MTL::Library* defaultLibrary = _mDevice->newLibrary(filePath, &error);
    if (defaultLibrary == nil) {
        throw std::runtime_error("failed to initialize default library");
    }

    NS::String* functionName = NS::String::string( "add_arrays", NS::StringEncoding::UTF8StringEncoding );
    if (functionName == nil) {
        throw std::runtime_error("failed to initialize function name");
    }

    MTL::Function* addFunction = defaultLibrary->newFunction(functionName);
    if (addFunction == nil) {
        throw std::runtime_error("failed to initialize add function");
    }

    _mAddFunctionPSO = _mDevice->newComputePipelineState(addFunction, &error);
    if (_mAddFunctionPSO == nil) {
        throw std::runtime_error("failed to initialize add function PSO");
    }

    _mCommandQueue = _mDevice->newCommandQueue();
    if (_mCommandQueue == nil) {
        throw std::runtime_error("failed to initialize command queue");
    }

    _mBufferA = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mBufferB = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mBufferResult = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
}

void MetalAdder::PrepareData() {
    float buff1[arrayLength];
    float buff2[arrayLength];

    for (int i = 0; i < arrayLength; i++) {
        buff1[i] = float(rand())/float(RAND_MAX);
        buff2[i] = float(rand())/float(RAND_MAX);
    }

    memcpy(_mBufferA->contents(), buff1, arrayLength);
    memcpy(_mBufferB->contents(), buff2, arrayLength);
}

void MetalAdder::SendComputeCommand() {
    auto commandBuffer = _mCommandQueue->commandBuffer();
    if (commandBuffer == nil) {
        throw std::runtime_error("failed to initialize command buffer");
    }

    auto computeEncoder = commandBuffer->computeCommandEncoder();
    if (computeEncoder == nil) {
        throw std::runtime_error("failed to initialize compute encoder");
    }

    computeEncoder->setComputePipelineState(_mAddFunctionPSO);
    computeEncoder->setBuffer(_mBufferA, 0, 0);
    computeEncoder->setBuffer(_mBufferB, 0, 1);
    computeEncoder->setBuffer(_mBufferResult, 0, 2);

    MTL::Size gridSize = MTL::Size(arrayLength, 1, 1);

    NS::UInteger threadGroupSize = _mAddFunctionPSO->maxTotalThreadsPerThreadgroup();
    if (threadGroupSize > arrayLength) {
        threadGroupSize = arrayLength;
    }

    MTL::Size tGSize = MTL::Size(threadGroupSize, 1, 1);

    computeEncoder->dispatchThreads(gridSize, tGSize);

    computeEncoder->endEncoding();

    commandBuffer->commit();

    commandBuffer->waitUntilCompleted();
}

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
