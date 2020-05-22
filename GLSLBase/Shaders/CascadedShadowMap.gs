#version 450 core

#define NUM_CASCADES 3

layout(triangles, invocations = NUM_CASCADES) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 g_ViewProj[NUM_CASCADES];
uniform int g_ShadowMapIndex;

in VertexOut
{
	vec2 texcoord;
}gs_in[];

out GeometryOut
{
	vec2 texcoord;
}gs_out;

void main()
{
	for(int i = 0; i < gl_in.length(); ++i)
	{
		gl_Layer = gl_InvocationID + g_ShadowMapIndex;
		gl_Position = g_ViewProj[gl_InvocationID] * gl_in[i].gl_Position;
		gs_out.texcoord = gs_in[i].texcoord;
		EmitVertex();
	}
	EndPrimitive();
}
