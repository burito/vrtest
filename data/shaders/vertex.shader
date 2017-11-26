#version 410
uniform mat4 world;
uniform mat4 camera;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v3NormalIn;
layout(location = 2) in vec2 v2TexCoordsIn;
out vec2 v2TexCoord;
out vec3 v3Normal;
void main()
{
	v2TexCoord = v2TexCoordsIn;
	v3Normal = (world * vec4(v3NormalIn.xyz, 0)).xyz;
	gl_Position = camera * world * vec4(position.xyz, 1);
}

