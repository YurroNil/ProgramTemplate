// include/initer.h
#pragma once

namespace SimpleDrawingDemo {

class Initer {
public:
    static GLFWwindow* InitWindow();
    static void CleanResources(RenderData** data);
};
} // namespace SimpleDrawingDemo
