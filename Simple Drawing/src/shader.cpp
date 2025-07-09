// src/shader.cpp
#include "pch.h"
#include "shader.h"

using ifs = std::ifstream;

namespace SimpleDrawingDemo {

/* ------- 静态成员与方法 ------- */

// 解析着色器文件
string Shader::Load(const string& path) {
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
unsigned int Shader::Compile(const string& path, GLenum type) {
    string codeStr = Load(path);
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
unsigned int Shader::CreateComputeShader(const string& path) {
    unsigned int computeShader = Compile(path, GL_COMPUTE_SHADER);
    
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
    s_programID = glCreateProgram(); 
    std::vector<unsigned int> shaders;

    // 编译着色器
    if(!vsh_path.empty()) shaders.push_back(Compile(vsh_path, GL_VERTEX_SHADER));
    if(!fsh_path.empty()) shaders.push_back(Compile(fsh_path, GL_FRAGMENT_SHADER));
    if(!geo_path.empty()) shaders.push_back(Compile(geo_path, GL_GEOMETRY_SHADER));
    if(!csh_path.empty()) shaders.push_back(Compile(csh_path, GL_COMPUTE_SHADER));
    
    // 附加着色器
    for(auto shader : shaders) {
        glAttachShader(s_programID, shader);
    }
    
    // 链接程序
    glLinkProgram(s_programID);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(s_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(s_programID, 512, NULL, infoLog);
        std::cerr << "着色器程序链接错误:\n" << infoLog << std::endl;
    }
    
    // 清理着色器对象
    for(auto shader : shaders) {
        glDeleteShader(shader);
    }
    
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
} // namespace SimpleDrawingDemo
