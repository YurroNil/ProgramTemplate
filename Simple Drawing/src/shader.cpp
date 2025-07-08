// src/main.cpp
#include "pch.h"
#include "shader.h"

using ifs = std::ifstream;

namespace SimpleDrawingDemo {

/* ------- 静态成员与方法 ------- */

// 解析着色器文件
string Shader::Load(const string& path) {
    string code; ifs file;
    file.exceptions(ifs::failbit | ifs::badbit);
    // 调试
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf(); 
        file.close();
        code = stream.str();
    } catch (ifs::failure& e) {
        std::cerr << "[ERROR_SHADER] 文件读取失败" << path << std::endl;
        std::cerr << "错误信息: " << e.what() << std::endl;
    }
    return code;
}

// 编译着色器
unsigned int Shader::Compile(const string& path, GLenum type) {
    const char* code = Load(path).c_str();

    // 编译着色器
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glAttachShader(s_programID, shader);

    return shader;
}

// 创建着色器程序
unsigned int Shader::Create(
    const string& vsh_path, const string& fsh_path,
    const string& geo_path, const string& csh_path)
{

    // 验证
    if(vsh_path.empty() && fsh_path.empty() && geo_path.empty() && csh_path.empty()) {
        std::cerr << "[ERROR_SHADER] 着色器路径为空" << std::endl;
        return 0;
    }

    // 创建着色器程序
    s_programID = glCreateProgram(); std::vector<unsigned int> shaders;

    // 编译着色器
    if(!vsh_path.empty()) shaders.push_back(Compile(vsh_path, GL_VERTEX_SHADER));
    if(!fsh_path.empty()) shaders.push_back(Compile(fsh_path, GL_FRAGMENT_SHADER));
    if(!geo_path.empty()) shaders.push_back(Compile(geo_path, GL_GEOMETRY_SHADER));
    if(!csh_path.empty()) shaders.push_back(Compile(csh_path, GL_COMPUTE_SHADER));

    glLinkProgram(s_programID);

    // 清理着色器对象
    for(const auto& shader : shaders) glDeleteShader(shader);

    return s_programID;
}


/* ------- 实例成员与方法 ------- */

void Shader::Use() const {
    glUseProgram(s_programID);
}

// uniform int
void Shader::SetUniform1i(const string& name, int value) const {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniform1i(location, value);
}

// uniform float
void Shader::SetUniform1f(const string& name, float value) const {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniform1f(location, value);
}

// uniform vec2
void Shader::SetUniform2f(const string& name, const vec2& vector) const {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniform2fv(location, 1, &vector[0]);
}

// uniform vec3
void Shader::SetUniform3f(const string& name, const vec3& vector) const {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniform3fv(location, 1, &vector[0]);
}

// uniform vec4
void Shader::SetUniform4f(const string& name, const vec4& vector) const {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniform4fv(location, 1, &vector[0]);
}

// uniform Matix 4x4
void Shader::SetUniformMat4(const string& name, const mat4& matrix) {
    GLint location = glGetUniformLocation(s_programID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}
}