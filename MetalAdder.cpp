//
// Created by Daniel Levi-Minzi on 8/24/22.
//

#include "MetalAdder.h"
#define MTL_PRIVATE_IMPLEMENTATION

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
}
