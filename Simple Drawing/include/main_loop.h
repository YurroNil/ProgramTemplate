// include/main_loop.h
#pragma once

namespace SimpleDrawingDemo {
class MainLoop {
public:
    static void RenderLoop(GLFWwindow* window, RenderData* data);
    static void InputHandles(GLFWwindow* window);
    static void BeginFrame(GLFWwindow* window);
    static void EndFrame(GLFWwindow* window);
    
    // 鼠标位置回调
    static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
    
    // 窗口大小回调
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};
}