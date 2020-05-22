#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 g_ViewProj[6];
uniform int g_ShadowMapIndex;

in VertexOut
{
	vec2 texcoord;
}gs_in[];

out GeometryOut
{
	vec2 texcoord;
	vec3 worldPos;
}gs_out;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face + g_ShadowMapIndex * 6;
		for(int i = 0; i < 3; ++i)
		{
			gs_out.worldPos = gl_in[i].gl_Position.xyz;
			gs_out.texcoord = gs_in[i].texcoord;
			gl_Position = g_ViewProj[face] * vec4(gs_out.worldPos, 1.0f);
			EmitVertex();
		}
		EndPrimitive();
	}
}
