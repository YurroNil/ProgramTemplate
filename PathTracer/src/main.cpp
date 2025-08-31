// src/main.cpp
#include "pch.h"
#include "optix_backend.h"
#include "window.h"

int main() {
    using namespace std;
    
    try {
        // 创建OpenGL窗口 - 先于CUDA/OptiX初始化
        OpenGLWindow window(1280, 720, "PathTracer Demo");
        
        if (!window.init()) { cerr << "FATAL: Failed to init OpenGL window!" << endl; }
        
        // 初始化CUDA和OptiX
        OptixBackend backend;

        // 加载PTX模块
        if (!backend.loadPTXModule()) {
            cerr << "Failed to load PTX module!" << endl;
            return -1;
        }

        window.setCallbacks(backend);
        
        // 进入主循环
        window.run();
    }
    catch (const exception& e) {
        cerr << "Fatal Error: " << e.what() << endl;
        return -1;
    }

    return 0;
}
