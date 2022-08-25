//
// Created by Daniel Levi-Minzi on 8/24/22.
//

#ifndef INC_00_METALADDER_H
#define INC_00_METALADDER_H
#include <Metal/Metal.hpp>


class MetalAdder {
    MTL::Device *_mDevice;
    MTL::ComputePipelineState *_mAddFunctionPSO;
    MTL::CommandQueue *_mCommandQueue;

    MTL::Buffer *_mBufferA;
    MTL::Buffer *_mBufferB;
    MTL::Buffer *_mBufferResult;

public:
    MetalAdder(MTL::Device *device);
};


#endif //INC_00_METALADDER_H
