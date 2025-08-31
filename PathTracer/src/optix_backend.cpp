// src/optix_backend.cpp
#include "pch.h"
#include "optix_backend.h"
#include "defines.h"

// 解决undefined reference to `g_optixFunctionTable_105`的问题.
extern "C" { OptixFunctionTable g_optixFunctionTable_105 = {}; }

OptixBackend::OptixBackend() {
    if (!init()) {
        throw std::runtime_error("Failed to init OptiX backend");
    }
}

OptixBackend::~OptixBackend() {
    cleanup();
}

bool OptixBackend::init() {
    // 初始化CUDA
    if (!initCUDA()) {
        std::cerr << "CUDA init failed" << std::endl;
        return false;
    }
    
    // 初始化OptiX
    if (!initOptiX()) {
        std::cerr << "OptiX init failed" << std::endl;
        return false;
    }

    std::cout << "Using device: " << deviceNames[selectedDevice] << "\n" << std::endl;
    
    return true;
}

bool OptixBackend::initCUDA() {
    // 初始化CUDA驱动API
    CUresult cuResult = cuInit(0);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuInit failed: " << errorName << std::endl;
        return false;
    }
    
    // 获取设备数量
    int deviceCount = 0;
    cuResult = cuDeviceGetCount(&deviceCount);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuDeviceGetCount failed: " << errorName << std::endl;
        return false;
    }
    if (deviceCount == 0) {
        std::cerr << "NoCUDAdevices found" << std::endl;
        return false;
    }
    
    // 打印找到的设备
    // std::cout << "\nFound " << deviceCount << "CUDAdevices" << std::endl;
    
    // 枚举设备信息
    for (int i = 0; i < deviceCount; ++i) {
        CUdevice device;
        cuResult = cuDeviceGet(&device, i);
        if (cuResult != CUDA_SUCCESS) {
            const char* errorName;
            cuGetErrorName(cuResult, &errorName);
            std::cerr << "cuDeviceGet failed for device " << i << ": " << errorName << std::endl;
            continue;
        }
        
        char name[256];
        cuResult = cuDeviceGetName(name, sizeof(name), device);
        if (cuResult != CUDA_SUCCESS) {
            const char* errorName;
            cuGetErrorName(cuResult, &errorName);
            std::cerr << "cuDeviceGetName failed for device " << i << ": " << errorName << std::endl;
            continue;
        }
        
        deviceNames.push_back(name);
        // 设备列表中显示的项
        // std::cout << "  Device " << i << ": " << name << std::endl;
    }
    
    if (deviceNames.empty()) {
        std::cerr << "No validCUDAdevices found" << std::endl;
        return false;
    }
    
    // 选择第一个设备
    CUdevice cuDevice;
    cuResult = cuDeviceGet(&cuDevice, selectedDevice);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "Failed to getCUDAdevice " << selectedDevice << ": " << errorName << std::endl;
        return false;
    }
    
    // 创建CUDA上下文
    cuResult = cuCtxCreate(&cuContext, CU_CTX_SCHED_AUTO, cuDevice);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuCtxCreate failed: " << errorName << std::endl;
        return false;
    }

    // 设置当前上下文
    cuResult = cuCtxSetCurrent(cuContext);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuCtxSetCurrent failed: " << errorName << std::endl;
        return false;
    }
    
    // 打印CUDA驱动版本
    int driverVersion;
    cuResult = cuDriverGetVersion(&driverVersion);
    if (cuResult == CUDA_SUCCESS) {
        std::cout << "\nCUDA driver version: " << driverVersion / 1000 << "." << (driverVersion % 1000) / 10 << std::endl;
    } else {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuDriverGetVersion failed: " << errorName << std::endl;
    }

    cudaError_t cudaStatus = cudaSetDevice(selectedDevice);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaSetDevice failed: " << cudaGetErrorString(cudaStatus) << std::endl;
    }
    
    return true;
}

bool OptixBackend::initOptiX() {

    // 初始化optix上下文
    OptixResult initResult = optixInit();
    if (initResult != OPTIX_SUCCESS) {
        std::cerr << "optixInit failed: " << optixGetErrorName(initResult) << std::endl;
        return false;
    }

    // 创建OptiX设备上下文
    OptixDeviceContextOptions options = {};
    options.logCallbackFunction = [](unsigned int level, const char* tag, const char* message, void*) {
        std::cerr << "[" << level << "][" << tag << "]: " << message << std::endl;
    };
    options.logCallbackLevel = 3; // 警告和错误
    
    OptixResult result = optixDeviceContextCreate(cuContext, &options, &optixContext);
    if (result != OPTIX_SUCCESS) {
        std::cerr << "optixDeviceContextCreate failed: " << optixGetErrorName(result) << std::endl;
        return false;
    }
    
    // 获取OptiX版本（使用宏）
    const unsigned int version = OPTIX_VERSION;
    const unsigned int major = version / 10000;
    const unsigned int minor = (version % 10000) / 100;
    const unsigned int micro = version % 100;
    
    std::cout << "OptiX inited. Version: " << major << "." << minor << "." << micro << std::endl;
    
    return true;
}

void OptixBackend::cleanup() {
    if (optixContext) {
        optixDeviceContextDestroy(optixContext);
        optixContext = nullptr;
    }
    
    if (cuContext) {
        cuCtxDestroy(cuContext);
        cuContext = nullptr;
    }
}
// 打印设备信息(调试)
void OptixBackend::printDeviceInfo() {
    std::cout << "\nAvailableCUDAdevices:" << std::endl;
    int index = 0;
    for (const auto& name : getDeviceNames()) {
        std::cout << "  [" << index++ << "] " << name << std::endl;
    }
}

bool OptixBackend::loadPTXModule() {
    // 读取PTX文件
    const string ptxPath = PTX_PATH + string("simple_render.ptx");
    std::ifstream ptxFile(ptxPath, std::ios::binary);
    
    if (!ptxFile) {
        std::cerr << "Failed to open PTX file: " << ptxPath << std::endl;
        return false;
    }
    
    // 将文件内容读入字符串
    std::stringstream buffer;
    buffer << ptxFile.rdbuf();
    std::string ptxSource = buffer.str();
    
    // 加载PTX模块
    CUresult cuResult = cuModuleLoadDataEx(&cudaModule, ptxSource.c_str(), 0, nullptr, nullptr);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuModuleLoadDataEx failed: " << errorName << std::endl;
        return false;
    }
    
    // 获取渲染内核函数
    cuResult = cuModuleGetFunction(&renderKernel, cudaModule, "renderKernel");
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuModuleGetFunction failed: " << errorName << std::endl;
        return false;
    }
    
    std::cout << "Loaded PTX module successfully: " << ptxPath << std::endl;
    return true;
}

void OptixBackend::createDisplayTexture(int width, int height) {
    if (textureWidth == width && textureHeight == height && displayTexture != 0) {
        return;
    }
    
    // 删除旧纹理（如果存在）
    if (displayTexture != 0) {
        // 确保有当前的CUDA上下文
        CUresult cuResult = cuCtxSetCurrent(cuContext);
        if (cuResult != CUDA_SUCCESS) {
            const char* errorName;
            cuGetErrorName(cuResult, &errorName);
            std::cerr << "cuCtxSetCurrent failed: " << errorName << std::endl;
        } else if (cudaTextureResource) {
            // 确保资源未被映射
            cudaError_t cudaStatus = cudaGraphicsGLRegisterImage(
            &cudaTextureResource, displayTexture, GL_TEXTURE_2D, 
            cudaGraphicsRegisterFlagsSurfaceLoadStore);
            if (cudaStatus != cudaSuccess && cudaStatus != cudaErrorNotMapped) {
                std::cerr << "cudaGraphicsUnmapResources failed: " << cudaGetErrorString(cudaStatus) << std::endl;
            }
            
            // 取消注册资源
            cudaStatus = cudaGraphicsUnregisterResource(cudaTextureResource);
            if (cudaStatus != cudaSuccess) {
                std::cerr << "cudaGraphicsUnregisterResource failed: " << cudaGetErrorString(cudaStatus) << std::endl;
            }
            cudaTextureResource = nullptr;
        }
        
        glDeleteTextures(1, &displayTexture);
        displayTexture = 0;
    }
    
    // 创建新纹理
    glGenTextures(1, &displayTexture);
    if (displayTexture == 0) {
        std::cerr << "Failed to generate OpenGL texture" << std::endl;
        return;
    }
    
    glBindTexture(GL_TEXTURE_2D, displayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // 检查 OpenGL 错误
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        std::cerr << "OpenGL error after glTexImage2D: " << glError << std::endl;
    }
    
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // 设置当前CUDA上下文
    CUresult cuResult = cuCtxSetCurrent(cuContext);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuCtxSetCurrent failed: " << errorName << std::endl;
        return;
    }

    // 检查CUDA和 OpenGL 互操作性
    int cudaDeviceCount;
    cudaGetDeviceCount(&cudaDeviceCount);
    for (int i = 0; i < cudaDeviceCount; i++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        std::cout << "Device " << i << ": " << prop.name 
                  << ", Can Access GL: " << prop.canMapHostMemory 
                  << ", Unified Addressing: " << prop.unifiedAddressing << std::endl;
    }

    // 注册纹理到 CUDA
    cudaError_t cudaStatus = cudaGraphicsGLRegisterImage(
        &cudaTextureResource, displayTexture, GL_TEXTURE_2D, cudaGraphicsRegisterFlagsWriteDiscard);
    
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaGraphicsGLRegisterImage failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        
        // 尝试使用不同的标志
        cudaStatus = cudaGraphicsGLRegisterImage(
            &cudaTextureResource, displayTexture, GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone);
        
        if (cudaStatus != cudaSuccess) {
            std::cerr << "Second attempt with different flags also failed: " << cudaGetErrorString(cudaStatus) << std::endl;
            cudaTextureResource = nullptr;
        } else {
            std::cout << "Registered texture withCUDAsuccessfully (second attempt)" << std::endl;
        }
    } else {
        std::cout << "Registered texture withCUDAsuccessfully" << std::endl;
    }
    
    textureWidth = width;
    textureHeight = height;
    
    std::cout << "Created display texture: " << width << "x" << height << std::endl;
}

void OptixBackend::updateDisplayTexture() {
    static float totalTime = 0.0f;
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    // 计算经过的时间
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = currentTime - startTime;
    totalTime = duration.count();
    
    if (!cudaTextureResource) {
        std::cerr << "Missing cudaTextureResource - recreating..." << std::endl;
        createDisplayTexture(textureWidth, textureHeight);
        if (!cudaTextureResource) {
            return; // 如果仍然失败，则退出
        }
    }
    
    // 设置当前CUDA上下文
    CUresult cuResult = cuCtxSetCurrent(cuContext);
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuCtxSetCurrent failed: " << errorName << std::endl;
        return;
    }
    
    // 映射CUDA资源
    cudaError_t cudaStatus = cudaGraphicsMapResources(1, &cudaTextureResource);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaGraphicsMapResources failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        return;
    }
    
    // 获取CUDA数组
    cudaArray_t textureArray;
    cudaStatus = cudaGraphicsSubResourceGetMappedArray(&textureArray, cudaTextureResource, 0, 0);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaGraphicsSubResourceGetMappedArray failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTextureResource);
        return;
    }
    
    // 创建CUDA表面
    cudaResourceDesc resDesc = {};
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = textureArray;
    
    cudaSurfaceObject_t surfaceObject;
    cudaStatus = cudaCreateSurfaceObject(&surfaceObject, &resDesc);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaCreateSurfaceObject failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTextureResource);
        return;
    }
    
    // 设置内核参数
    void* args[] = { &surfaceObject, &textureWidth, &textureHeight, &totalTime };
    
    // 启动内核
    dim3 blockSize(16, 16);
    dim3 gridSize((textureWidth + blockSize.x - 1) / blockSize.x, 
                  (textureHeight + blockSize.y - 1) / blockSize.y);
    
    cuResult = cuLaunchKernel(
        renderKernel, gridSize.x, gridSize.y,
        1, blockSize.x, blockSize.y,
        1, 0, nullptr, args, nullptr
    );
    
    if (cuResult != CUDA_SUCCESS) {
        const char* errorName;
        cuGetErrorName(cuResult, &errorName);
        std::cerr << "cuLaunchKernel failed: " << errorName << std::endl;
    }
    
    // 清理资源
    cudaStatus = cudaDestroySurfaceObject(surfaceObject);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaDestroySurfaceObject failed: " << cudaGetErrorString(cudaStatus) << std::endl;
    }
    
    cudaStatus = cudaGraphicsUnmapResources(1, &cudaTextureResource);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaGraphicsUnmapResources failed: " << cudaGetErrorString(cudaStatus) << std::endl;
    }
}