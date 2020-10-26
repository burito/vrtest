#version 410 core

uniform mat4 modelview;
uniform mat4 projection;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout (location = 0) out vec3 fragNormal;
layout (location = 1) out vec2 fragUV;

void main()
{
	fragUV = inUV;
	fragNormal = mat3(modelview) * inNormal;
	gl_Position = projection * modelview * vec4(inPosition, 1);
}
