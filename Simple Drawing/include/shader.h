#pragma once
namespace SimpleDrawingDemo {

class RenderData;

struct Shader {

    /* ------- 静态成员与方法 ------- */
    inline static unsigned int s_programID = 0;

    static string Load(const string& path);
    static unsigned int Create(
        const string& vsh_path = "", const string& fsh_path = "",
        const string& geo_path = "", const string& csh_path = ""
    );
    static unsigned int Compile(const string& path, GLenum type);

    /* ------- 实例成员与方法 ------- */
    
    void Use() const;
    void SetUniform1i(const string& name, int value) const;
    void SetUniform1f(const string& name, float value) const;
    void SetUniform2f(const string& name, const vec2& vector) const;
    void SetUniform3f(const string& name, const vec3& vector) const;
    void SetUniform4f(const string& name, const vec4& vector) const;
    void SetUniformMat4(const string& name, const mat4& matrix);
};
}