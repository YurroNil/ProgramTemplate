// src/main_loop.cpp
#include "pch.h"
#include "render_data.h"
#include "defines.h"
#include "main_loop.h"

namespace SimpleDrawingDemo {

void MainLoop::InputHandles(GLFWwindow* window) {
}

void MainLoop::BeginFrame(GLFWwindow* window) {
    // 清除颜色缓冲
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void MainLoop::EndFrame(GLFWwindow* window) {
    // 交换缓冲并处理事件
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// 渲染循环
void MainLoop::RenderLoop(GLFWwindow* window, RenderData* data) {

    BeginFrame(window);

    // 使用着色器程序
    glUseProgram(data->shader->s_programID);
    
    // 设置线条颜色（通过uniform）
    int colorLoc = glGetUniformLocation(data->shader->s_programID, "uColor");
    glUniform3f(colorLoc, 1.0f, 0.5f, 0.2f); // RGB颜色值（橙色）

    // 绑定VAO并绘制
    glBindVertexArray(data->VAO);
    glDrawArrays(GL_LINE_STRIP, 0, data->segments); // 绘制连续线段

    EndFrame(window);    
}
}