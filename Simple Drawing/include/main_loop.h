// include/main_loop.h
#pragma once

namespace SimpleDrawingDemo {
class MainLoop {
public:
    static void RenderLoop(GLFWwindow* window, RenderData* data);
    static void InputHandles(GLFWwindow* window);
    static void BeginFrame(GLFWwindow* window);
    static void EndFrame(GLFWwindow* window);
};
}
