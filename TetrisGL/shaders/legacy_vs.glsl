#version 120

attribute vec3 aPos;
attribute vec3 aColor;

uniform mat4 uMVP; // Model-View-Projection matrix

varying vec3 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
}
