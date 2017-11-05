#version 410
uniform mat4 matrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v3NormalIn;
layout(location = 2) in vec2 v2TexCoordsIn;
out vec2 v2TexCoord;
out vec3 v3Normal;
void main()
{
	v2TexCoord = v2TexCoordsIn;
	v3Normal = v3NormalIn;
	gl_Position = matrix * vec4(position.xyz, 1);
}

