#include "vector_addition.h"

#define CARD 1
#define MAX_SOURCE_SIZE (0x100000)

int VectorAddition::setupCL()
{
	clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of Platforms: %d\n\n", numPlatforms);

	platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
	clGetPlatformIDs(numPlatforms, platforms, NULL);

	clGetDeviceIDs(platforms[CARD], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

	devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
	clGetDeviceIDs(platforms[CARD], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	cmdQueue = clCreateCommandQueue(context, devices[CARD], 0, &status);

	bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);

	clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL);
	clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL);

	return status;
}

void VectorAddition::runCLKernels()
{
	FILE *fp;
	errno_t err;
	char *source_str;
	size_t source_size;

	err = fopen_s(&fp, "kernel.cl", "r");
	if (err) {
		fprintf(stderr, "HIBA: Failed to open file\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Create a program with source code
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &status);
	// Build (compile) the program for the device
	clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

	kernel = clCreateKernel(program, "vectoradd", &status);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

	size_t globalWorkSize[1];
	globalWorkSize[0] = elements;

	clEnqueueNDRangeKernel(cmdQueue, kernel, 1, //1-dimension
		NULL, globalWorkSize, NULL, 0, NULL, NULL); //local sizes, wait lists, etc...

	clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL);
}
void VectorAddition::printPlatformInfo()
{
	clGetPlatformInfo(platforms[0], CL_PLATFORM_PROFILE, NULL, profile, &size);

	profile = (char*)malloc(size);
	clGetPlatformInfo(platforms[0], CL_PLATFORM_PROFILE, size, profile, NULL);

	clGetPlatformInfo(platforms[0], CL_PLATFORM_VERSION, NULL, platform_version, &size);

	platform_version = (char*)malloc(size);
	clGetPlatformInfo(platforms[0], CL_PLATFORM_VERSION, size, platform_version, NULL);

	printf("Platform Informations:\n");
	printf("(Status now: %d)\n", status);
	printf("Profle: %s\n", profile);
	printf("Platform Version: %s\n", platform_version);

}
void VectorAddition::printDeviceInfo()
{

	clGetDeviceInfo(devices[0], CL_DEVICE_VENDOR, NULL, NULL, &size);
	vendor = (char*)malloc(sizeof(char)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_VENDOR, size, vendor, NULL);

	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, NULL, NULL, &size);
	device_name = (char*)malloc(sizeof(char)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, size, device_name, NULL);

	clGetDeviceInfo(devices[0], CL_DEVICE_OPENCL_C_VERSION, NULL, NULL, &size);
	open_cl_c_version = (char*)malloc(sizeof(char)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_OPENCL_C_VERSION, size, open_cl_c_version, NULL);

	clGetDeviceInfo(devices[0], CL_DEVICE_VERSION, NULL, NULL, &size);
	open_cl_version = (char*)malloc(sizeof(char)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_VERSION, size, open_cl_version, NULL);

	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, NULL, NULL, &size);
	max_workgroup = (size_t*)malloc(sizeof(size_t)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, size, &max_workgroup, NULL);

	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, NULL, NULL, &size);
	max_cu = (cl_uint*)malloc(sizeof(cl_uint)*size);
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, size, &max_cu, NULL);

	printf("\n\nDevice Informations:\n");
	printf("Vendor: %s\n", vendor);
	printf("Device: %s\n", device_name);
	printf("openCL C: %s\n", open_cl_c_version);
	printf("openCL: %s\n", open_cl_version);
	printf("Max WorkGroup: %d\n", max_workgroup);
	printf("Max ComputeUnits: : %d\n\n", max_cu);
}

void VectorAddition::checkError()
{
	int result = 1;
	for (int i = 0; i<elements; i++) {
		if (C[i] != i + i) {
			result = 0;
			break;
		}
	}

	if (result) {
		printf("Output is correct\n");
	}
	else {
		printf("Output is incorrect\n");
	}
}

int main()
{
	VectorAddition vektor;
	vektor.setupCL();

	vektor.printPlatformInfo();
	vektor.printDeviceInfo();
	vektor.runCLKernels();

	vektor.checkError();
	return 0;
}
