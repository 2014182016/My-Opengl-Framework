#version 450 core

#include "Material.glsl"

in GeometryOut
{
	vec2 texcoord;
}fs_in;

void main()
{
	vec4 diffuse = g_Material.m_Diffuse;
	if(g_Material.m_UseDiffuseMap)
	{
		diffuse *= texture(g_DiffuseMap, fs_in.texcoord);
		if(diffuse.a < 0.1f)
			discard;
	}
}
