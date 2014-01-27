#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#include "CL/cl.hpp"
#include <vector>

namespace oul {

class Context {
    public:
        Context(std::vector<cl::Device> devices, bool OpenGLInterop, bool profilingEnabled);
        void createProgramFromSource(std::string filename, std::string buildOptions = "");
        void createProgramFromBinary(std::string filename, std::string buildOptions = "");
    private:
        cl::Context context;
        std::vector<cl::CommandQueue> queues;
        std::vector<cl::Program> programs;
        std::vector<cl::Device> devices;
        cl::Platform platform;
};

};

#endif /* CONTEXT_HPP_ */
