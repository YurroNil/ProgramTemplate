// src/initer.cpp
#include "pch.h"
#include "render_data.h"
#include "defines.h"
#include "initer.h"

namespace SimpleDrawingDemo {

// 窗口创建
GLFWwindow* Initer::InitWindow() {
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // 指定OpenGL版本为4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello, OpenGL!", NULL, NULL);
    glfwMakeContextCurrent(window);

    // 初始化GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    return window;
}

// 资源清理
void Initer::CleanResources(RenderData** data) {
    delete *data; *data = nullptr;

    glfwTerminate();
}
}   // namespace SimpleDrawingDemo
