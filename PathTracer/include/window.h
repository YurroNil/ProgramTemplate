// include/window.h
#pragma once
#include "optix_backend.h"

class OpenGLWindow {
public:
    using KeyCallback = std::function<void(int, int, int)>;
    using MouseCallback = std::function<void(double, double)>;
    using ScrollCallback = std::function<void(double, double)>;
    using FramebufferSizeCallback = std::function<void(int, int)>;
    using RenderCallback = std::function<void()>;

    OpenGLWindow(int width = 1920, int height = 1080, const char* title = "PathTracer Demo");
    ~OpenGLWindow();

    bool init(); bool initScreenQuad();
    void run(); void shutdown();
    
    string LoadShader(const string& path);
    unsigned int CompileShader(const string& path, GLenum type);
    unsigned int CreateComputeShader(const string& path);
    unsigned int CreateShader(
        const string& vsh_path = "", const string& fsh_path = "",
        const string& geo_path = "", const string& csh_path = ""
    );

    // 设置回调函数
    void setCallbacks(OptixBackend& backend);
    void setKeyCallback(KeyCallback callback) { keyCallback = callback; }
    void setMouseCallback(MouseCallback callback) { mouseCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { scrollCallback = callback; }
    void setFramebufferSizeCallback(FramebufferSizeCallback callback) { framebufferSizeCallback = callback; }
    void setRenderCallback(RenderCallback callback) { renderCallback = callback; }

    // 获取窗口信息
    GLFWwindow* getWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    glm::vec2 getMousePosition() const { return {mouseX, mouseY}; }
    double getDeltaTime() const { return deltaTime; }
    double getFPS() const { return fps; }

private:
    GLFWwindow* window = nullptr;
    int width, height;
    const char* title;
    
    // 输入状态
    double mouseX = 0.0, mouseY = 0.0;
    bool firstMouse = true;
    
    // 时间统计
    double lastFrameTime = 0.0;
    double deltaTime = 0.0;
    double fps = 0.0;
    int frameCount = 0;
    double lastFpsUpdate = 0.0;
    unsigned int screenShaderProgram;
    GLuint quadVAO, quadVBO;
    
    // 回调函数
    KeyCallback keyCallback;
    MouseCallback mouseCallback;
    ScrollCallback scrollCallback;
    FramebufferSizeCallback framebufferSizeCallback;
    RenderCallback renderCallback;

    // GLFW回调的静态包装函数
    static void framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);
    static void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
    
    // 初始化OpenGL状态
    void initOpenGLState();
};
