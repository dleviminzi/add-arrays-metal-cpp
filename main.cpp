#include <iostream>

#define CA_PRIVATE_IMPLEMENTATION
// #include <Metal/Metal.hpp>
//#include <AppKit/AppKit.hpp>
#include "MetalAdder.h"


std::string StringFrom(NS::String *name) {

    std::string s;

    for (int i = 0; i < name->length(); i++) {
        auto c = name->character(i);
        s.push_back(char(c)); // convert short to char and push to string
    };

    return s;
}

int main() {
    MTL::Device *device = MTL::CreateSystemDefaultDevice();
    NS::String *name = device->name();
    std::cout << "Hello from " << StringFrom(name) << std::endl;

    auto m = MetalAdder(device);
    m.PrepareData();
    m.SendComputeCommand();
    m.Verify();


    device->release();
    return 0;
}
