#version 450 core

#include "PassConstants.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

out VertexOut
{
	vec3 color;
}vs_out;

void main()
{
	gl_Position = g_ViewProj * vec4(a_Position, 1.0f);
	vs_out.color = a_Color;
}
