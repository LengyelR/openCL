#include <CL/cl.h>
#include<stdio.h>
#include<stdlib.h>

#define NUM_OF_ELEMENTS 2048

class VectorAddition
{
    protected:
    
        const int elements;
    
        size_t datasize;
        size_t size;

        // Host buffers
        int *A;
        int *B;
        int *C;

        // Device buffers
        cl_mem bufA;
        cl_mem bufB;
        cl_mem bufC;
        
        // CL objects
		cl_int status;
        cl_uint numPlatforms;
        cl_uint numDevices;
        cl_platform_id *platforms;
        cl_device_id *devices;
        cl_context context;
        cl_command_queue cmdQueue;
        cl_program program;
		cl_kernel kernel;

		//Info Objects(platform)
		char *profile;
		char *platform_version;

		//Info Objects(device)
		char *vendor;					//CL_DEVICE_VENDOR
		char *device_name;			 //CL_DEVICE_NAME
		char *open_cl_c_version;			//CL_DEVICE_OPENCL_C_VERSION  ---- openCL C
		char *open_cl_version;			// CL_DEVICE_VERSION --- openCL
		size_t *max_workgroup;		// CL_DEVICE_MAX_WORK_GROUP_SIZE
		cl_uint *max_cu;					//CL_DEVICE_MAX_COMPUTE_UNITS
		cl_uint *max_freq;				//CL_DEVICE_MAX_CLOCK_FREQUENCY
        
    public:
        VectorAddition()
              : elements(NUM_OF_ELEMENTS),
                datasize(sizeof(int)*elements),
				A((int*)malloc(datasize)), //ezekek szintén?
				B((int*)malloc(datasize)),
				C((int*)malloc(datasize)),
				numPlatforms(0),
				platforms(NULL), // ez jo-e igy?
				numDevices(0),

				//PlatformInfo
				profile(NULL),
				platform_version(NULL),

				//DeviceInfo
				devices(NULL),
				vendor(NULL),
				device_name(NULL),
				open_cl_c_version(NULL),
				open_cl_version(NULL),
				max_workgroup(NULL),
				max_cu(NULL),
				max_freq(NULL)

        {
			for(int i=0; i<elements; i++) {
				A[i]=i;
				B[i]=i;
			}

        }
            
        ~VectorAddition() // nem tom hogy ez itt jo lesz-e
        {
			// Free OpenCL resources
			clReleaseKernel(kernel);
			clReleaseProgram(program);
			clReleaseCommandQueue(cmdQueue);
			clReleaseMemObject(bufA);
			clReleaseMemObject(bufB);
			clReleaseMemObject(bufC);
			clReleaseContext(context);

			// Free host resources
			free(A);
			free(B);
			free(C);
			free(platforms);
			free(devices);
		}
        
        int setupVectorAddition();
        
        int setupCL(); // Set up context, Devices, CmdQue and MemBuffs
        void runCLKernels(); // Set kernel args, enque them in CmdQue, wait, start etc...
        void printDeviceInfo();
		void printPlatformInfo();
		void checkError(); 
    
};