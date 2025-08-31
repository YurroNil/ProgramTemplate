// src/window.cpp
#include "pch.h"
#include "window.h"
#include "defines.h"

OpenGLWindow::OpenGLWindow(int width, int height, const char* title) 
    : width(width), height(height), title(title) {}

OpenGLWindow::~OpenGLWindow() {
    shutdown();
}

bool OpenGLWindow::init() {
    // 确保GLFW未初始化
    
    if (!glfwInit()) {
        std::cerr << "[INITER_ERROR] GLFW初始化失败" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 设置错误回调以获取更多信息
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
    });

    // 设置OpenGL版本和配置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE); // 确保窗口可见
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    
    // 尝试创建窗口
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        
        // 尝试降低OpenGL版本要求
        std::cerr << "Trying with OpenGL 4.5...\n";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }
    
    if (!window) {
        std::cerr << "Failed to create GLFW window even with OpenGL 4.5\n";
        
        // 尝试使用兼容模式
        std::cerr << "Trying with compatibility profile...\n";
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }
    
    if (!window) {
        std::cerr << "Failed to create GLFW window with compatibility profile\n";
        glfwTerminate();
        return false;
    }

    // 设置上下文
    glfwMakeContextCurrent(window);

    // 设置窗口用户指针
    glfwSetWindowUserPointer(window, this);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        
        // 尝试重新加载
        if (!gladLoadGL()) {
            std::cerr << "Failed to reload GLAD\n";
            glfwTerminate();
            return false;
        }
    }

    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallbackWrapper);
    glfwSetKeyCallback(window, keyCallbackWrapper);
    glfwSetCursorPosCallback(window, mouseCallbackWrapper);
    glfwSetScrollCallback(window, scrollCallbackWrapper);

    // 启用原始鼠标运动
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    
    // 显示窗口
    glfwShowWindow(window);
    
    // 初始化OpenGL状态
    initOpenGLState();

    // 打印OpenGL信息
    std::cout << "\nOpenGL Inited Successfully:\n";
    std::cout << "  Version: " << glGetString(GL_VERSION) << "\n";

    // 检查实际OpenGL版本
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "  Actual OpenGL Version: " << major << "." << minor << "\n";

    return true;
}

void OpenGLWindow::initOpenGLState() {
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // 启用多重采样
    glEnable(GL_MULTISAMPLE);
    
    // 开启面剔除
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // 混合设置
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 设置清屏颜色
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
}

void OpenGLWindow::run() {
    lastFrameTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        // 计算帧时间
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        
        // 更新FPS计数
        frameCount++;
        if (currentTime - lastFpsUpdate >= 1.0) {
            fps = frameCount / (currentTime - lastFpsUpdate);
            frameCount = 0;
            lastFpsUpdate = currentTime;
            
            // 设置FPS(之后尝试在程序内显示)
            string titleWithFPS = "FPS: " + std::to_string(static_cast<int>(fps));
        }
        
        // 清屏
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 调用渲染回调
        if (renderCallback) {
            renderCallback();
        }
        
        // 交换缓冲区和处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void OpenGLWindow::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool OpenGLWindow::initScreenQuad() {
    // 顶点数据 (位置, 纹理坐标)
    float vertices[] = {
        // 位置       // 纹理坐标
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    // 创建顶点数组对象和顶点缓冲对象
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // 创建全屏渲染着色器
    screenShaderProgram = CreateShader(
        SHADER_PATH + string("general.frag"),
        SHADER_PATH + string("general.vert")
    );
    
    return true;
}

using ifs = std::ifstream;

// 解析着色器文件
string OpenGLWindow::LoadShader(const string& path) {
    string code; ifs file;
    file.exceptions(ifs::failbit | ifs::badbit);
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf(); 
        file.close();
        code = stream.str();
    } catch (ifs::failure& e) {
        std::cerr << "[ERROR_SHADER] 文件读取失败: " << path << std::endl;
        std::cerr << "错误信息: " << e.what() << std::endl;
    }
    return code;
}

// 编译着色器
unsigned int OpenGLWindow::CompileShader(const string& path, GLenum type) {
    string codeStr = LoadShader(path);
    const char* code = codeStr.c_str();

    // 编译着色器
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    
    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "着色器编译错误 (" << path << "):\n" << infoLog << std::endl;
    }
    
    return shader;
}

// 创建计算着色器程序
unsigned int OpenGLWindow::CreateComputeShader(const string& path) {
    unsigned int computeShader = CompileShader(path, GL_COMPUTE_SHADER);
    
    // 创建着色器程序
    unsigned int program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "计算着色器链接错误:\n" << infoLog << std::endl;
    }
    
    glDeleteShader(computeShader);
    return program;
}

// 创建着色器程序
unsigned int OpenGLWindow::CreateShader(
    const string& vsh_path, const string& fsh_path,
    const string& geo_path, const string& csh_path)
{
    // 验证
    if(vsh_path.empty() && fsh_path.empty() && geo_path.empty() && csh_path.empty()) {
        std::cerr << "[ERROR_SHADER] 着色器路径为空" << std::endl;
        return 0;
    }

    // 创建着色器程序
    unsigned int program_id = glCreateProgram(); 
    std::vector<unsigned int> shaders;

    // 编译着色器
    if(!vsh_path.empty()) shaders.push_back(CompileShader(vsh_path, GL_VERTEX_SHADER));
    if(!fsh_path.empty()) shaders.push_back(CompileShader(fsh_path, GL_FRAGMENT_SHADER));
    if(!geo_path.empty()) shaders.push_back(CompileShader(geo_path, GL_GEOMETRY_SHADER));
    if(!csh_path.empty()) shaders.push_back(CompileShader(csh_path, GL_COMPUTE_SHADER));
    
    // 附加着色器
    for(auto shader : shaders) { glAttachShader(program_id, shader); }
    
    // 链接程序
    glLinkProgram(program_id);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, 512, NULL, infoLog);
        std::cerr << "着色器程序链接错误:\n" << infoLog << std::endl;
    }
    
    // 清理着色器对象
    for(auto shader : shaders) { glDeleteShader(shader); }
    
    return program_id;
}

void OpenGLWindow::setCallbacks(OptixBackend& backend) {
    // 设置窗口回调
        setKeyCallback([&](int key, int scancode, int action) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(getWindow(), GLFW_TRUE);
            }
            
            // 添加F11全屏切换
            if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
                static bool fullscreen = false;
                static int savedWidth = getWidth();
                static int savedHeight = getHeight();
                
                if (!fullscreen) {
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(getWindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                } else {
                    glfwSetWindowMonitor(getWindow(), nullptr, 100, 100, savedWidth, savedHeight, GLFW_DONT_CARE);
                }
                fullscreen = !fullscreen;
            }
        });
        
        setMouseCallback([&](double xoffset, double yoffset) {
            // 这里可以添加相机控制逻辑
        });
        
        setScrollCallback([&](double xoffset, double yoffset) {
            // 这里可以添加缩放逻辑
        });
        
        setFramebufferSizeCallback([&](int width, int height) {
            std::cout << "Window resized to: " << width << "x" << height << std::endl;
        });
        
        setRenderCallback([&]() {
            static bool firstFrame = true;
        
            // 获取窗口尺寸
            int width, height;
            glfwGetFramebufferSize(getWindow(), &width, &height);
            
            // 创建或更新显示纹理
            if (firstFrame || width != backend.getTextureWidth() || height != backend.getTextureHeight()) {
                backend.createDisplayTexture(width, height);
                firstFrame = false;
            }
            
            // 更新纹理内容
            backend.updateDisplayTexture();
            
            // 使用现代OpenGL渲染全屏四边形
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            // 使用着色器程序
            glUseProgram(screenShaderProgram);
            
            // 绑定纹理
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, backend.getDisplayTexture());
            glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTexture"), 0);
            
            // 绘制全屏四边形
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            
            // 恢复状态
            glEnable(GL_DEPTH_TEST);
        });
}

// 静态回调包装函数
void OpenGLWindow::framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height) {
    OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->width = width;
        instance->height = height;
        glViewport(0, 0, width, height);
        if (instance->framebufferSizeCallback) {
            instance->framebufferSizeCallback(width, height);
        }
    }
}

void OpenGLWindow::keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(window));
    if (instance && instance->keyCallback && action != GLFW_REPEAT) {
        instance->keyCallback(key, scancode, action);
    }
}

void OpenGLWindow::mouseCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(window));
    if (instance) {
        if (instance->firstMouse) {
            instance->mouseX = xpos;
            instance->mouseY = ypos;
            instance->firstMouse = false;
        }
        
        double xoffset = xpos - instance->mouseX;
        double yoffset = instance->mouseY - ypos; // 反转Y轴
        
        instance->mouseX = xpos;
        instance->mouseY = ypos;
        
        if (instance->mouseCallback) {
            instance->mouseCallback(xoffset, yoffset);
        }
    }
}

void OpenGLWindow::scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset) {
    OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(window));
    if (instance && instance->scrollCallback) {
        instance->scrollCallback(xoffset, yoffset);
    }
}