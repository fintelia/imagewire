#version 330
#extension GL_ARB_explicit_uniform_location : enable

out vec4 OutputColor;

layout(location = 0) uniform vec2 windowSize;
layout(location = 1) uniform sampler2D image;
layout(location = 2) uniform vec2 imageSize;

void main() {
	vec2 texCoord = gl_FragCoord.xy / windowSize;
	texCoord.y = 1.0 - texCoord.y;

	float windowAspect = windowSize.x / windowSize.y;
	float imageAspect = imageSize.x / imageSize.y;
	if(windowAspect > imageAspect) {
		texCoord.x = (texCoord.x - 0.5) *  windowAspect / imageAspect + 0.5;
		if(texCoord.x < 0 || texCoord.x > 1)
			discard;
	} else {
		texCoord.y = (texCoord.y - 0.5) * imageAspect / windowAspect + 0.5;
		if(texCoord.y < 0 || texCoord.y > 1)
			discard;
	}

	if(fract(texCoord.x) > 0.5)
		discard;
	OutputColor = vec4(texture(image, texCoord).rgb, 1);
}
