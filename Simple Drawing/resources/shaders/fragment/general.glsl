// resources/shaders/fragment/general.glsl

#version 330 core
out vec4 FragColor;
uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0); // 设置带透明度的颜色
}
