#include "catch.hpp"

#include "TestFixture.hpp"
#include "OpenCLManager.hpp"
#include "HistogramPyramids.hpp"
#include "OulConfig.hpp"

namespace test
{

void compileCode(oul::Context &context) {
    context.createProgramFromSource(std::string(OUL_DIR)+"/HistogramPyramids.cl", "-I \"" + std::string(OUL_DIR) + "\"");
}

TEST_CASE("Histogram Pyramid OpenCL code compiles") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform
    for(int i = 0; i < platformDevices.size(); i++) {
        oul::Context context = oul::opencl()->createContext(platformDevices[i].second, false);
        INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>())
        CHECK_NOTHROW(compileCode(context));
    }
}

TEST_CASE("3D Histogram Pyramid Buffer create") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);
            int sizeX = 64;
            int sizeY = 64;
            int sizeZ = 64;
            const int size = sizeX*sizeY*sizeZ;
            unsigned char * data = new unsigned char[size]();
            cl::Buffer buffer = cl::Buffer(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    sizeof(char)*size,
                    data
            );
            delete[] data;
            oul::HistogramPyramid3DBuffer hp(context);
            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK_NOTHROW(hp.create(buffer, sizeX, sizeY, sizeZ));
        }
    }
}

TEST_CASE("2D Histogram Pyramid create") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);

            int sizeX = 32;
            int sizeY = 32;
            unsigned char * data = new unsigned char[sizeX*sizeY]();
            cl::Image2D image = cl::Image2D(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
                    sizeX, sizeY,
                    0,
                    data
            );
            oul::HistogramPyramid2D hp(context);
            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK_NOTHROW(hp.create(image, sizeX, sizeY));
            delete[] data;
        }
    }
}


TEST_CASE("3D Histogram Pyramid create") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);

            // Check to see if the device has 3D texture writing capabilities
            if(device.getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_3d_image_writes") == std::string::npos)
                continue;

            int sizeX = 32;
            int sizeY = 32;
            int sizeZ = 32;
            unsigned char * data = new unsigned char[sizeX*sizeY*sizeZ]();
            cl::Image3D image = cl::Image3D(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
                    sizeX, sizeY, sizeZ,
                    0, 0,
                    data
            );
            delete[] data;
            oul::HistogramPyramid3D hp(context);
            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK_NOTHROW(hp.create(image, sizeX, sizeY, sizeZ));
        }
    }
}

unsigned char * createRandomData(unsigned int size, unsigned int * sum) {
    unsigned char * data = new unsigned char[size];
    // Populate with random data
    srand(time(NULL));
    unsigned int correctSum = 0;
    for(int i = 0; i < size; i++) {
        unsigned char value = rand()%2;
        data[i] = value;
        correctSum += value;
    }

    *sum = correctSum;

    return data;
}

TEST_CASE("3D Histogram Pyramid Buffer Sum") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);

            unsigned int sizeX = 64;
            unsigned int sizeY = 64;
            unsigned int sizeZ = 64;
            unsigned int size = sizeX*sizeY*sizeZ;
            unsigned int correctSum = 0;
            unsigned char * data = createRandomData(size, &correctSum);
            cl::Buffer buffer = cl::Buffer(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    sizeof(char)*size,
                    data
            );
            delete[] data;
            oul::HistogramPyramid3DBuffer hp(context);
            hp.create(buffer, sizeX, sizeY, sizeZ);

            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK(hp.getSum() == correctSum);
        }
    }
}


TEST_CASE("2D Histogram Pyramid Sum") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);

            int sizeX = 64;
            int sizeY = 64;
            unsigned int correctSum = 0;
            unsigned char * data = createRandomData(sizeX*sizeY, &correctSum);

            cl::Image2D image = cl::Image2D(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
                    sizeX, sizeY,
                    0,
                    data
            );
            delete[] data;
            oul::HistogramPyramid2D hp(context);
            hp.create(image, sizeX, sizeY);

            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK(hp.getSum() == correctSum);
        }
    }
}


TEST_CASE("3D Histogram Pyramid Sum") {
    oul::TestFixture fixture;
    std::vector<oul::PlatformDevices> platformDevices = fixture.getAllDevices();
    // For every platform and every device:
    for(int i = 0; i < platformDevices.size(); i++) {
        for(int j = 0; j < platformDevices[i].second.size(); j++) {
            cl::Device device = platformDevices[i].second[j];
            oul::Context context = oul::opencl()->createContext(device, false);

            // Check to see if the device has 3D texture writing capabilities
            if(device.getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_3d_image_writes") == std::string::npos)
                continue;


            int sizeX = 64;
            int sizeY = 64;
            int sizeZ = 64;
            unsigned int correctSum = 0;
            unsigned char * data = createRandomData(sizeX*sizeY*sizeZ, &correctSum);

            cl::Image3D image = cl::Image3D(
                    context.getContext(),
                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                    cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
                    sizeX, sizeY, sizeZ,
                    0, 0,
                    data
            );
            delete[] data;
            oul::HistogramPyramid3D hp(context);
            hp.create(image, sizeX, sizeY, sizeZ);

            INFO("Device: " << device.getInfo<CL_DEVICE_NAME>());
            INFO("Platform: " << platformDevices[i].first.getInfo<CL_PLATFORM_NAME>());
            CHECK(hp.getSum() == correctSum);
        }
    }
}

} // end namespace