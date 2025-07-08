// src/main.cpp
#include "pch.h"
#include "render_data.h"
#include "initer.h"
#include "main_loop.h"
#include "shader.h"

using namespace SimpleDrawingDemo;

int main() {

/* --------------创建器-------------- */
    // 创建窗口
    GLFWwindow* window = Initer::InitWindow();

    // 创建渲染数据实例
    RenderData* data = RenderData::CreateInst();

    data->Draw();

/* --------------渲染循环-------------- */

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        MainLoop::InputHandles(window);
        MainLoop::RenderLoop(window, data);
    }

/* --------------资源清理-------------- */
    Initer::CleanResources(&data);

    return 0;
}
