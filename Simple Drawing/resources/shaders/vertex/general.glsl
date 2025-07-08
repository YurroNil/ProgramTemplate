// resources/shaders/vertex/general.glsl

#version 330 core
layout (location = 0) in vec2 aPos; // 二维位置属性

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
 