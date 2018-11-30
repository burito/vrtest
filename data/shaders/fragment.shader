#version 410 core
uniform sampler2D diffuse;

layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = texture( diffuse, fragUV);

	float ld = dot(fragNormal, vec3(0,-1,0));

	outColor = vec4(fragNormal, 1);
}