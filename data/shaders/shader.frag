#version 410 core

uniform sampler2D diffuse;

layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

void main()
{
	// test if there is a texture bound
	if( textureSize(diffuse, 0).x > 1 )
	{
		outColor = texture( diffuse, vec2(fragUV.x, 1.0 - fragUV.y));
		outColor = outColor * fragNormal.z + vec4(fragNormal*0.5, 1);
		float ld = dot(fragNormal, vec3(0,-1,0));
//		outColor = outColor * ld;
	}
	else
	{	// render it with normals, because normals are cool
		outColor = vec4(fragNormal, 1);
	}
}
