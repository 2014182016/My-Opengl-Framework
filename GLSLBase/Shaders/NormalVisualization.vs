#version 450 core

#include "PassConstants.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

layout(location = 0) uniform mat4 g_World;

out VertexOut
{
	vec3 normal;
}vs_out;

void main()
{
	gl_Position = g_ViewProj * g_World * vec4(a_Position, 1.0f);
	vs_out.normal = a_Normal;
}
