// src/render_cpp
#include "pch.h"
#include "render_data.h"
#include "shader.h"
#include "defines.h"

namespace SimpleDrawingDemo {

RenderData::RenderData(int segments, int vertex_size, ShaderPaths& shader_paths)
    : shader(new Shader()), segments(segments), vertices(new float[segments * vertex_size])
{
    shader->s_programID = Shader::Create(
        shader_paths.vsh_path, shader_paths.fsh_path,
        shader_paths.gsh_path, shader_paths.csh_path
    );
}

RenderData::~RenderData() {

    // 删除顶点与顶点缓冲数据
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader->s_programID);

    // 删除实例
    delete[] vertices;
    delete shader;
}

// 顶点对象绑定处理
void RenderData::BindVertexObjects() {
    if(vertices == nullptr) std::cerr << "[BinderFn] 空指针异常, vertices指针大小: " << sizeof(vertices) << std::endl;
    
    // 创建VAO和VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定缓冲
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

// 三次函数计算
float CubicFn(float x) {
    return -2.0f * pow(x, 3) + 5.0f * pow(x, 2) - 7.0f * x + 9.0f;
}

// 绘制命令
void RenderData::Draw() {
    // 绑定顶点对象
    BindVertexObjects();

    // 生成顶点数据（将函数值映射到[-1, 1]范围）
    const float x_min = -2.0f, x_max = 4.0f; // x取值范围

    for(int i = 0; i < segments; ++i) {
        float x = x_min + (x_max - x_min) * i / (segments - 1);
        float y = CubicFn(x);
        
        // 归一化处理（根据函数值范围调整）
        vertices[i*2] = (x - x_min)/(x_max - x_min)*2 - 1; // x映射到[-1,1]
        vertices[i*2 + 1] = y / 20.0f; // y值缩放（根据实际需要调整除数）
    }
}

RenderData* RenderData::CreateInst() {
    const unsigned int segments = 100;
    const string general_path = string("general.glsl");
    ShaderPaths shader_paths = { FSH_PATH + general_path, VSH_PATH + general_path, "", "" };

    return new RenderData(segments, segments * 2, shader_paths);
}
}   // namespace SimpleDrawingDemo
