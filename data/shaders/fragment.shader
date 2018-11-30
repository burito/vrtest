#version 410 core
uniform sampler2D diffuse;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = texture( diffuse, inUV);

	float ld = dot(inNormal, vec3(0,-1,0));

	outColor = vec4(inNormal, 1);
}