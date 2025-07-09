// src/main.cpp
#include "pch.h"
#include "render_data.h"
#include "initer.h"
#include "main_loop.h"
#include "shader.h"

using namespace SimpleDrawingDemo;

int main() {
    // 创建窗口
    GLFWwindow* window = Initer::InitWindow();
    
    // 获取渲染数据实例并初始化
    RenderData* data = RenderData::GetInstance();
    data->screenWidth = 1920;
    data->screenHeight = 1080;
    data->InitRayTracingResources();
    
    // 设置鼠标回调
    glfwSetCursorPosCallback(window, MainLoop::MousePosCallback);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        MainLoop::InputHandles(window);
        MainLoop::RenderLoop(window, data);
    }

    // 资源清理
    Initer::CleanResources(&data);

    return 0;
}