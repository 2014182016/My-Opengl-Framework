#version 450 core

#include "PassConstants.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_Texcoord;

layout(location = 0) uniform mat4 g_World;

out VertexOut
{
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
	vec2 texcoord;
}vs_out;

void main()
{
	vs_out.worldPos = vec3(g_World * vec4(a_Position, 1.0f));
	gl_Position = g_ViewProj * vec4(vs_out.worldPos, 1.0f);
	vs_out.texcoord = a_Texcoord;
	vs_out.normal = a_Normal * transpose(inverse(mat3(g_World)));
	vs_out.tangent = a_Tangent * transpose(inverse(mat3(g_World)));
}
