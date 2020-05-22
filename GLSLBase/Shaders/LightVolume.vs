#version 450 core

#include "PassConstants.glsl"

layout(location = 0) in vec3 a_Position;

layout(location = 0) uniform mat4 g_World;

void main()
{
	gl_Position = g_ViewProj * g_World * vec4(a_Position, 1.0f);
}
