// include/optix_backend.h
#pragma once

class OptixBackend {
public:
    OptixBackend();
    ~OptixBackend();
    
    bool init();
    void cleanup();
    
    // 获取设备信息
    const std::vector<string>& getDeviceNames() const { return deviceNames; }
    int getSelectedDevice() const { return selectedDevice; }
    int getTextureWidth() const { return textureWidth; }
    int getTextureHeight() const { return textureHeight; }
    GLuint getDisplayTexture() const { return displayTexture; }

    void printDeviceInfo();
    bool initOptiX();
    bool initCUDA();
    bool loadPTXModule();
    void createDisplayTexture(int width, int height);
    void updateDisplayTexture();

private:

    CUcontext cuContext = nullptr;
    OptixDeviceContext optixContext = nullptr;
    
    int selectedDevice = 0, textureWidth = 0, textureHeight = 0;
    std::vector<string> deviceNames;
    // 添加以下成员变量
    CUmodule cudaModule = nullptr;
    CUfunction renderKernel = nullptr;
    cudaGraphicsResource* cudaTextureResource = nullptr;
    GLuint displayTexture = 0;
};

// 错误检查
#define CHECK_CUDA(call) \
do { \
    CUresult result = (call); \
    if (result != CUDA_SUCCESS) { \
        const char* errorName; \
        cuGetErrorName(result, &errorName); \
        std::cerr << "CUDA error at " << __FILE__ << ":" << __LINE__ << ": " << errorName << std::endl; \
        return false; \
    } \
} while(0)
