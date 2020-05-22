#include "stdafx.h"
#include "Material.h"
#include "Shader.h"

void Material::Bind(Shader* shader) const
{
	if (shader == nullptr)
		return;

	shader->SetUniform("g_Material.m_Diffuse", m_Diffuse);
	shader->SetUniform("g_Material.m_Specular", m_Specular);
	shader->SetUniform("g_Material.m_Roughness", m_Roughness);
	shader->SetUniform("g_Material.m_Emissive", m_Emissive);
	shader->SetUniform("g_Material.m_Metallic", m_Metallic);

	shader->SetUniform("g_Material.m_DiffuseMap", 0);
	shader->SetUniform("g_Material.m_NormalMap", 1);

	shader->SetUniform("g_Material.m_UseDiffuseMap", m_DiffuseMap != 0);
	shader->BindTexture("g_DiffuseMap", 0, m_DiffuseMap);

	shader->SetUniform("g_Material.m_UseNormalMap", m_NormalMap != 0);
	shader->BindTexture("g_NormalMap", 1, m_NormalMap);

	shader->SetUniform("g_Material.m_UseSpecularMap", m_SpecularMap != 0);
	shader->BindTexture("g_SpecularMap", 2, m_SpecularMap);

	shader->SetUniform("g_Material.m_UseRoughnessMap", m_RoughnessMap != 0);
	shader->BindTexture("g_RoughnessMap", 3, m_RoughnessMap);

	shader->SetUniform("g_Material.m_UseMetallicMap", m_MetallicMap != 0);
	shader->BindTexture("g_MetallicMap", 4, m_MetallicMap);

	shader->SetUniform("g_Material.m_UseAOMap", m_AOMap != 0);
	shader->BindTexture("g_AOMap", 5, m_AOMap);
}