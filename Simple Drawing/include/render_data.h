// include/render_data.h
#pragma once
#include "shader_paths.h"
#include "shader.h"

namespace SimpleDrawingDemo {

struct RenderData {
    unsigned int VAO, VBO, segments;
    float* vertices; Shader* shader;

    RenderData(int segments, int vertex_size, ShaderPaths& shader_paths);
    ~RenderData();

    void BindVertexObjects();
    void Draw();
    static RenderData* CreateInst();
};
}
