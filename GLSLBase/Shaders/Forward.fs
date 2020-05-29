#version 450 core

#include "Lighting.glsl"
#include "Material.glsl"
#include "PassConstants.glsl"

in VertexOut
{
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
	vec2 texcoord;
}fs_in;

layout(location = 0) out vec4 out_FragColor;
layout(location = 1) out vec4 out_Brightness;

void main()
{
	vec4 diffuse = g_Material.m_Diffuse;
	if(g_Material.m_UseDiffuseMap)
	{
		diffuse *= texture(g_DiffuseMap, fs_in.texcoord);
		if(diffuse.a < 0.1f)
			discard;
	}

	vec3 normal = normalize(fs_in.normal);
	if(g_Material.m_UseNormalMap)
	{
		vec3 normalSample = texture(g_NormalMap, fs_in.texcoord).xyz;
		normalSample = 2.0f * normalSample - 1.0f;
		vec3 tangent = normalize(fs_in.tangent - dot(fs_in.tangent, normal) * normal);
		vec3 bitangent = cross(normal, tangent);
		mat3 TBN = transpose(mat3(tangent, bitangent, normal));
		normal = normalize(normalSample * TBN);
	}

	vec3 specular= g_Material.m_Specular;
	if(g_Material.m_UseSpecularMap)
	{
		specular *= texture(g_SpecularMap, fs_in.texcoord).rgb;
	}

	float roughness = g_Material.m_Roughness;
	if(g_Material.m_UseRoughnessMap)
	{
		roughness *= texture(g_RoughnessMap, fs_in.texcoord).r;
	}

	float metallic = g_Material.m_Metallic;
	if(g_Material.m_UseSpecularMap)
	{
		metallic *= texture(g_MetallicMap, fs_in.texcoord).r;
	}

	float ao = 1.0f;
	if(g_Material.m_UseAOMap)
	{
		ao = texture(g_AOMap, fs_in.texcoord).r;
	}

	MaterialData material = { diffuse.rgb, metallic, specular, roughness };

	vec3 toEye = normalize(g_EyePos - fs_in.worldPos);
	vec3 directLight = ComputeLightings(g_Lights, material, fs_in.worldPos, normal, toEye, g_EyePos);
	vec3 litColor = g_Ambient.rgb * diffuse.rgb + (directLight * ao) + g_Material.m_Emissive;

	out_FragColor = vec4(litColor, diffuse.a);

	float brightness = dot(out_FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0f)
        out_Brightness = vec4(out_FragColor.rgb, 1.0f);
    else
        out_Brightness = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
