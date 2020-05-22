
struct Material
{
	vec4 m_Diffuse;
	vec3 m_Specular;
	float m_Roughness;
	vec3 m_Emissive;
	float m_Metallic;

	bool m_UseDiffuseMap;
	bool m_UseNormalMap;
	bool m_UseSpecularMap;
	bool m_UseRoughnessMap;
	bool m_UseMetallicMap;
	bool m_UseAOMap;
};

uniform Material g_Material;

uniform sampler2D g_DiffuseMap;
uniform sampler2D g_NormalMap;
uniform sampler2D g_SpecularMap;
uniform sampler2D g_RoughnessMap;
uniform sampler2D g_MetallicMap;
uniform sampler2D g_AOMap;