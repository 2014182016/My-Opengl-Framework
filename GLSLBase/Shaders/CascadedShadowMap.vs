#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 3) in vec2 a_Texcoord;

layout(location = 0) uniform mat4 g_World;

out VertexOut
{
	vec2 texcoord;
}vs_out;

void main()
{
	gl_Position = g_World * vec4(a_Position, 1.0f);
	vs_out.texcoord = a_Texcoord;
}
