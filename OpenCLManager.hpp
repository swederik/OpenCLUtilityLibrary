#ifndef OPENCLMANAGER_HPP_
#define OPENCLMANAGER_HPP_

#include "CL/OpenCL.hpp"
#include <vector>
#include "Context.hpp"
#include "DeviceCriteria.hpp"
#include "Exceptions.hpp"


namespace oul {

/**
 * Singleton class which is used mainly for creating OpenCL contexts in an easy way
 */
class OpenCLManager
{
public:
	static OpenCLManager * getInstance();
	static void shutdown();

	Context createContext(std::vector<cl::Device> &devices, bool OpenGLInterop, bool profilingEnabled);
	Context createContext(int argc, char ** argv, DeviceCriteria &defaultCriteria);
	Context createContext(const DeviceCriteria &criteria);
	std::vector<cl::Device> getDevices(const DeviceCriteria &criteria);
	std::vector<cl::Platform> getPlatforms(oul::DevicePlatform platformCriteria);
	static void setDebugMode(bool mode);
	static bool getDebugMode();
	static bool deviceHasOpenGLInteropCapability(const cl::Device &device);
	static bool devicePlatformMismatch(const cl::Device &device, const cl::Platform &platform);
private:
	OpenCLManager();
	std::vector<cl::Platform> platforms;

	static bool debugMode;
	static OpenCLManager * instance;

    void sortDevicesAccordingToPreference(
        int numberOfPlatforms,
        int maxNumberOfDevices,
        std::vector<cl::Device> * platformDevices,
        DevicePreference preference,
        std::vector<cl::Device> * sortedPlatformDevices,
        int * platformScores
    );
};

OpenCLManager* opencl(); //Shortcut for accessing the OpenCLManager

}; // namespace oul

#endif /* OPENCLMANAGER_HPP_ */
