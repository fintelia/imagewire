#version 330
#extension GL_ARB_explicit_uniform_location : enable

out vec4 OutputColor;

layout(location = 0) uniform vec2 windowSize;
layout(location = 1) uniform sampler2D image;

void main() {
    OutputColor = vec4(1,0,0,1);
}
