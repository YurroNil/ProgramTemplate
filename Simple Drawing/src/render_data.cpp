// src/render_data.cpp
#include "pch.h"
#include "render_data.h"
#include "shader.h"
#include "defines.h"

namespace SimpleDrawingDemo {

// 单例实例
RenderData* RenderData::instance = nullptr;

RenderData::RenderData() 
    : shader(new Shader()), screenWidth(1920), screenHeight(1080),
      cameraPos(0.0f, 0.0f, 3.0f), cameraFront(0.0f, 0.0f, -1.0f), cameraUp(0.0f, 1.0f, 0.0f),
      outputTexture(0), quadVAO(0), quadVBO(0), computeShaderID(0) 
{
    // 初始化顶点数组（空）
    vertices = nullptr;
    segments = 0;
}

RenderData::~RenderData() {
    // 删除光线追踪资源
    glDeleteTextures(1, &outputTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(computeShaderID);
    
    // 删除着色器程序
    if (shader) {
        glDeleteProgram(shader->s_programID);
        delete shader;
    }
    
    // 删除顶点数据
    if (vertices) {
        delete[] vertices;
    }
}

// 初始化光线追踪资源
void RenderData::InitRayTracingResources() {
    // 创建计算着色器
    computeShaderID = Shader::CreateComputeShader(CSH_PATH + string("ray_tracing.glsl"));
    
    // 创建输出纹理
    glGenTextures(1, &outputTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    // 创建全屏四边形
    float quadVertices[] = {
        // 位置       // 纹理坐标
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    // 创建全屏渲染着色器
    const string general_path = string("general.glsl");
    ShaderPaths shader_paths = { FSH_PATH + general_path, VSH_PATH + general_path, "", "" };
    shader->s_programID = Shader::Create(
        shader_paths.vsh_path, shader_paths.fsh_path,
        shader_paths.gsh_path, shader_paths.csh_path
    );
    
    // 设置纹理采样器
    glUseProgram(shader->s_programID);
    glUniform1i(glGetUniformLocation(shader->s_programID, "screenTexture"), 0);
}

RenderData* RenderData::GetInstance() {
    if (!instance) {
        instance = new RenderData();
    }
    return instance;
}
}   // namespace SimpleDrawingDemo