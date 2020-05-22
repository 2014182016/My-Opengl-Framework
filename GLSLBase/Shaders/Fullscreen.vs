#version 450 core

#include "PassConstants.glsl"

out VertexOut
{
	vec2 texcoord;
}vs_out;

const vec2 g_TexCoords[6] =
{
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f)
};

const vec2 g_Positions[6] =
{
	vec2(-1.0f, -1.0f),
	vec2(1.0f, -1.0f),
	vec2(-1.0f, 1.0f),
	vec2(-1.0f, 1.0f),
	vec2(1.0f, -1.0f),
	vec2(1.0f, 1.0f),
};

void main()
{
	vs_out.texcoord = g_TexCoords[gl_VertexID];
	gl_Position  = vec4(g_Positions[gl_VertexID], 0.0f, 1.0f);
}
