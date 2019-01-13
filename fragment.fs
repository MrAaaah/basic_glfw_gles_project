#version 100

precision mediump float;

varying mediump vec2 uv;

void main() {
   gl_FragColor = vec4(uv, 0.0, 1.0);
}
