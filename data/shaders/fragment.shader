#version 410 core
uniform sampler2D diffuse;
in vec2 v2TexCoord;
in vec3 v3Normal;
out vec4 outputColor;
void main()
{
//	outputColor = texture( diffuse, v2TexCoord);
	outputColor = vec4(v3Normal, 1.0);
}