// srsc/kernels/simple_render.cu
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

// 简单的CUDA渲染内核 - 生成渐变背景
extern "C" __global__ void renderKernel(cudaSurfaceObject_t output, int width, int height, float time) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        // 计算UV坐标 (0-1范围)
        float u = static_cast<float>(x) / width;
        float v = static_cast<float>(y) / height;
        
        // 创建随时间变化的渐变背景
        float r = 255 * (0.5f + 0.5f * sinf(u * 10.0f + time));
        float g = 255 * (0.5f + 0.5f * sinf(v * 8.0f + time * 1.5f));
        float b = 255 * (0.5f + 0.5f * sinf((u + v) * 6.0f + time * 2.0f));
        
        uchar4 color = {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            255
        };
        
        // 使用表面写入函数
        surf2Dwrite(color, output, x * sizeof(uchar4), y);
    }
}