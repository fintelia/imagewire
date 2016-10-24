#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform vec2 windowSize;
layout(location = 1) uniform sampler2D heightmap;
layout(location = 2) uniform uvec2 resolution;
layout(location = 3) uniform vec2 world_size;
layout(location = 4) uniform mat4 viewProjectionMatrix;
layout(location = 5) uniform vec3 eye;

uniform vec2 offset[6] = vec2[](vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(1, 0),
                                vec2(0, 1), vec2(1, 1));

const float height_scale = 2147483.648;

out vec3 position;

vec3 toWorldCoordinates(float north, float west, float altitude) {
	const float earthRadius = 6371008.8;

	float r = earthRadius + altitude;
	float latitude = (north-eye.x) / earthRadius;
	float longitude = (west-eye.z) / earthRadius;

	float x = r * sin(latitude);
	float z = r * sin(longitude);
	return vec3(x + eye.x, sqrt(r*r - x*x - z*z) - earthRadius, z + eye.z);
}

void main()
{
	uint qid = uint(gl_VertexID) / 6u;
	uint qoffset = uint(gl_VertexID) % 6u;
	vec2 pos = vec2(qid % (resolution.x - 1u), qid / (resolution.x - 1u)) +
		offset[qoffset];

	float height = texelFetch(heightmap, ivec2(pos), 0).x * height_scale;
	pos *= world_size / (resolution - vec2(1));

	position = toWorldCoordinates(pos.x, pos.y, height);
    gl_Position = viewProjectionMatrix * vec4(position, 1);
}
