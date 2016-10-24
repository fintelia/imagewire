#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform vec2 windowSize;
layout(location = 1) uniform sampler2D heightmap;
layout(location = 2) uniform uvec2 resolution;
layout(location = 3) uniform vec2 world_size;
layout(location = 4) uniform mat4 viewProjectionMatrix;

uniform vec2 offset[6] = vec2[](vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(1, 0),
                                vec2(0, 1), vec2(1, 1));

const float height_scale = 2147483.648;

out vec3 position;

// mat4 computeVewMat() {
// 	vec3 f = normalize(fwd);
// 	vec3 UP = normalize(up);
// 	vec3 s = cross(f,UP);
// 	vec3 u = cross(normalize(s), f);
// 	return mat4();
// }

void main()
{
	// float aspect = windowSize.x / windowSize.y;
	// float pf = 1 / tan(fovy / 2);
	// mat4 projection =
	// 	mat4(pf / aspect, 0, 0, 0,                      // first column
	// 		 0, pf, 0, 0,                               // second column
	// 		 0, 0, (zfar + znear) / (znear - zfar), -1, // third column
	// 		 0, 0, 2*zfar*znear / (znear - zfar), 0);   // final column

	uint qid = uint(gl_VertexID) / 6u;
	uint qoffset = uint(gl_VertexID) % 6u;
	vec2 pos = vec2(qid % (resolution.x - 1u), qid / (resolution.x - 1u)) +
		offset[qoffset];

	float height = texelFetch(heightmap, ivec2(pos), 0).x * height_scale;
	pos *= world_size / (resolution - vec2(1));
	
    position = vec3(pos.x, height, pos.y);
    gl_Position = viewProjectionMatrix * vec4(position, 1);
}
