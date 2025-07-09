// src/main_loop.cpp
#include "pch.h"
#include "render_data.h"
#include "defines.h"
#include "main_loop.h"

namespace SimpleDrawingDemo {

// 窗口大小变化回调
void MainLoop::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    RenderData* data = RenderData::GetInstance();
    
    // 更新视口
    glViewport(0, 0, width, height);
    
    // 更新屏幕尺寸
    data->screenWidth = width;
    data->screenHeight = height;
    
    // 重新创建输出纹理
    glDeleteTextures(1, &data->outputTexture);
    glGenTextures(1, &data->outputTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data->outputTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, data->outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    std::cout << "窗口大小已更新: " << width << "x" << height << std::endl;
}

// 鼠标位置回调
void MainLoop::MousePosCallback(GLFWwindow* window, double xpos, double ypos) {
    RenderData* data = RenderData::GetInstance();
    if (!data->mouseCaptured) return;
    
    if (data->firstMouse) {
        data->lastX = xpos;
        data->lastY = ypos;
        data->firstMouse = false;
    }
    
    float xoffset = xpos - data->lastX;
    float yoffset = data->lastY - ypos; // 反转Y轴
    data->lastX = xpos;
    data->lastY = ypos;
    
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    // 使用四元数避免万向节死锁
    data->yaw += xoffset;
    data->pitch += yoffset;
    
    // 限制俯仰角
    if (data->pitch > 89.0f) data->pitch = 89.0f;
    if (data->pitch < -89.0f) data->pitch = -89.0f;
    
    using namespace glm;

    // 使用四元数计算旋转
    quat pitchQuat = angleAxis(radians(-data->pitch), vec3(1.0f, 0.0f, 0.0f));
    quat yawQuat = angleAxis(radians(data->yaw), vec3(0.0f, 1.0f, 0.0f));
    quat orientation = yawQuat * pitchQuat;
    orientation = normalize(orientation);
    
    // 计算摄像机方向
    // 根据欧拉角计算前向量
    vec3 front;
    front.x = cos(radians(data->yaw)) * cos(radians(data->pitch));
    front.y = sin(radians(data->pitch));
    front.z = sin(radians(data->yaw)) * cos(radians(data->pitch));
    data->cameraFront = normalize(front);

    // 重新计算右向量和上向量
    data->cameraRight = normalize(cross(data->cameraFront, data->worldUp));
    data->cameraUp = normalize(cross(data->cameraRight, data->cameraFront));
}

void MainLoop::InputHandles(GLFWwindow* window) {
    // 获取RenderData实例
    static RenderData* data = RenderData::GetInstance();
    static float cameraSpeed = 0.1f;
    
    // 处理ESC键切换鼠标捕获状态
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        // 防止重复触发
        static double lastPressTime = 0.0;
        double currentTime = glfwGetTime();
        
        if (currentTime - lastPressTime > 0.5) { // 0.5秒防抖
            data->mouseCaptured = !data->mouseCaptured;
            glfwSetInputMode(window, GLFW_CURSOR, 
                data->mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            
            if (data->mouseCaptured) {
                // 重新捕获鼠标时重置firstMouse标志
                data->firstMouse = true;
            }
            
            lastPressTime = currentTime;
            std::cout << "Mouse capture: " << (data->mouseCaptured ? "ON" : "OFF") << std::endl;
        }
    }
    
    // 仅在捕获鼠标时处理移动
    if (!data->mouseCaptured) return;
    
    // 相机控制 - 使用四元数计算的方向向量
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        data->cameraPos += cameraSpeed * data->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        data->cameraPos -= cameraSpeed * data->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        data->cameraPos -= cameraSpeed * data->cameraRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        data->cameraPos += cameraSpeed * data->cameraRight;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        data->cameraPos += cameraSpeed * data->worldUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        data->cameraPos -= cameraSpeed * data->worldUp;
}

void MainLoop::BeginFrame(GLFWwindow* window) {
    // 清除颜色缓冲
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
    
    // 更新光源位置（动画效果）
    float time = static_cast<float>(glfwGetTime());
    data->lightPos = vec3(
        3.0f * sin(time * 0.5f),
        4.0f + sin(time * 0.7f),
        -8.0f + cos(time * 0.5f)
    );
    
    // 使用计算着色器进行光线追踪
    glUseProgram(data->computeShaderID);
    
    // 设置uniform
    glUniform3f(glGetUniformLocation(data->computeShaderID, "cameraPos"), data->cameraPos.x, data->cameraPos.y, data->cameraPos.z);
    glUniform3f(glGetUniformLocation(data->computeShaderID, "cameraFront"), data->cameraFront.x, data->cameraFront.y, data->cameraFront.z);
    glUniform3f(glGetUniformLocation(data->computeShaderID, "cameraRight"), data->cameraRight.x, data->cameraRight.y, data->cameraRight.z);
    glUniform3f(glGetUniformLocation(data->computeShaderID, "cameraUp"), data->cameraUp.x, data->cameraUp.y, data->cameraUp.z);
    glUniform1f(glGetUniformLocation(data->computeShaderID, "time"), time);
    
    // 分派计算着色器
    glDispatchCompute((data->screenWidth + 15) / 16, (data->screenHeight + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    // 渲染全屏四边形
    glUseProgram(data->shader->s_programID);
    glBindVertexArray(data->quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data->outputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    EndFrame(window);    
}
} // namespace SimpleDrawingDemo
