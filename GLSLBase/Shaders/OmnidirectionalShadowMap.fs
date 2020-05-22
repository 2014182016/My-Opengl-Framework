#version 450 core

#include "Material.glsl"

in GeometryOut
{
	vec2 texcoord;
	vec3 worldPos;
}fs_in;

uniform vec3 g_LightPos;
uniform float g_Far;

void main()
{
	vec4 diffuse = g_Material.m_Diffuse;
	if(g_Material.m_UseDiffuseMap)
	{
		diffuse *= texture(g_DiffuseMap, fs_in.texcoord);
		if(diffuse.a < 0.1f)
			discard;
	}

	float lightDistance = length(fs_in.worldPos - g_LightPos);
	lightDistance /= g_Far;
	gl_FragDepth = lightDistance;
}
