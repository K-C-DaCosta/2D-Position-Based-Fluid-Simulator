
#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif

#include "CL/cl.h"
#include <string>
#include <vector>
#include <exception>

#ifndef OPENCL_UTIL_H
#define OPENCL_UTIL_H



typedef struct OpenCLInfoBlock_{
    cl_context context;
    std::vector<cl_device_id>   deviceIds;
    std::vector<cl_platform_id> platformIds;
}OpenCLInfoBlock;


class OCLException : public std::exception{
    cl_int error;
public:
    OCLException(cl_int e){
        error = e;
    }
    const char * what () const throw ();
};


/*An OpenCL wrapper for memmory allocation*/
template <class T>
class OpenCLBuffer{
public:
    OpenCLBuffer(uint64_t flags){
       clBufferID = NULL;
       this->flags = flags;
    }

    ~OpenCLBuffer(){
        if(clBufferID){
            clReleaseMemObject(clBufferID);
        }
    }

    void createServerBuffer(OpenCLInfoBlock & oclInfoBlock){
        auto dataSize = hostBuffer.size();
        auto context = oclInfoBlock.context;
        cl_int error = CL_SUCCESS;
        clBufferID = clCreateBuffer (context,flags,sizeof(T)*dataSize, hostBuffer.data(), &error);

        if(error != CL_SUCCESS){
            throw OCLException(error);
        }
    }

    void setKernelArg(cl_kernel kernel, cl_int arg){
         clSetKernelArg (kernel, arg, sizeof(cl_mem),&clBufferID);
    }

    uint32_t size(){
       return hostBuffer.size();
    }

    cl_mem* getID(){
        return &clBufferID;
    }

    void resize(uint32_t size){
        hostBuffer.resize(size);
    }

    std::vector<T> & getHostBuffer(){
        return hostBuffer;
    }

    T & operator [](int I){
        return hostBuffer[I];
    }

    cl_int readBackIntoMem(cl_command_queue queue,uint32_t enable_blocking){
         return clEnqueueReadBuffer(queue, clBufferID, enable_blocking, 0,sizeof(T)*hostBuffer.size(),hostBuffer.data (), 0, nullptr, nullptr);
    }

    void zero(){
        memset(hostBuffer.data(),0,hostBuffer.size()*sizeof(T));
    }

private:
    std::vector<T> hostBuffer;
    cl_mem clBufferID;
    uint64_t flags;
};

//More utility procedures for opencl
namespace CLU {
    void setupOCL(OpenCLInfoBlock *oclInfo);
    std::string GetPlatformName (cl_platform_id id);
    std::string GetDeviceName (cl_device_id id);
    void CheckError (cl_int error);
    cl_program createProgram (OpenCLInfoBlock & ib,const std::string& source);
    cl_kernel createKernel(OpenCLInfoBlock & ib, cl_program program,std::string progName);
    cl_command_queue createCommandQueue(OpenCLInfoBlock & ib);
    void enqueueNDRangeKernel(cl_command_queue /* command_queue */,
                           cl_kernel        /* kernel */,
                           cl_uint          /* work_dim */,
                           const size_t *   /* global_work_offset */,
                           const size_t *   /* global_work_size */,
                           const size_t *   /* local_work_size */,
                           cl_uint          /* num_events_in_wait_list */,
                           const cl_event * /* event_wait_list */,
                           cl_event *       /* event */);

    std::string getProgramBuildInfo (cl_program  program,
       cl_device_id  device,
       cl_program_build_info  param_name);


    void buildProgram(OpenCLInfoBlock & ib,cl_program prog);
    const char * getErrorString(cl_int error);
}


#endif // OPENCL_UTIL_H
