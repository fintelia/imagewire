#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform vec2 windowSize;
layout(location = 5) uniform vec3 eye;

in vec3 position;
out vec4 OutputColor;

void main() {
	float depth = distance(position, eye);
    OutputColor = vec4(0,0.5,0.2,1);//vec4(position.xy * 0.5 + vec2(0.5),0,1);

	vec2 partials = vec2(dFdx(depth), dFdy(depth));
	
	OutputColor = vec4(vec3(0,0.5,0.2) * depth * 0.0001, 1);
}
