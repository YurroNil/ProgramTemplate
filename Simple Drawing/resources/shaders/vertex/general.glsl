// resources/shaders/vertex/general.glsl
#version 450 core
layout (location = 0) in vec2 aPos; // 位置属性
layout (location = 1) in vec2 aTexCoords; // 纹理坐标

out vec2 TexCoords;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}
