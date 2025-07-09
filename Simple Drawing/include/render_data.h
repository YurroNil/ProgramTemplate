// include/render_data.h
#pragma once
#include "shader_paths.h"
#include "shader.h"

namespace SimpleDrawingDemo {

struct RenderData {
    // 光线追踪资源
    unsigned int outputTexture;
    unsigned int quadVAO, quadVBO;
    unsigned int computeShaderID;
    
    // 相机参数
    vec3 cameraPos;
    vec3 cameraFront;
    vec3 cameraRight;
    vec3 cameraUp;
    vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
    vec3 lightPos;
    
    // 屏幕尺寸
    int screenWidth;
    int screenHeight;
    
    // 摄像机控制参数
    bool mouseCaptured = true;  // 鼠标是否被捕获
    bool firstMouse = true;      // 是否是第一次获取鼠标位置
    float lastX = 0.0f;          // 上一帧鼠标X位置
    float lastY = 0.0f;          // 上一帧鼠标Y位置
    float yaw = -90.0f;          // 偏航角 (yaw)
    float pitch = 0.0f;          // 俯仰角 (pitch)
    
    // 原始绘图资源（保留但不再使用）
    unsigned int VAO, VBO, segments;
    float* vertices; 
    Shader* shader;

    // 单例模式
    static RenderData* instance;
    static RenderData* GetInstance();
    
    RenderData();
    ~RenderData();
    
    void InitRayTracingResources();
    
    // 原始方法（保留但不再使用）
    void BindVertexObjects() {}
    void Draw() {}
};
}