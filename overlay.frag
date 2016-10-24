#version 330
#extension GL_ARB_explicit_uniform_location : enable

out vec4 OutputColor;

layout(location = 0) uniform vec2 windowSize;
layout(location = 1) uniform sampler2D image;

void main() {
	vec2 texCoord = gl_FragCoord.xy / windowSize * vec2(1,-1);
	
	OutputColor = vec4(texture(image, texCoord).rgb, 1);
}
