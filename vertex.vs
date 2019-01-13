#version 100

attribute vec4 position;

varying mediump vec2 uv;

uniform mat4 rotation_mat;

void main() {
    uv = position.xy + vec2(0.5, 0.5);
    gl_Position = rotation_mat * position;
}
