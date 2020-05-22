#include "stdafx.h"
#include "Light.h"
#include "Shader.h"
#include "Renderer.h"
#include "Scene.h"

int Light::lightNum = 0;

Light::Light(UniformBuffer<LightConstants>* buffer) : Object()
{ 
	m_LightConstants.m_LightData.m_LightIndex = m_LightIndex = lightNum++;
	m_LightUniformBuffer = buffer;

	// Create FBO
	glGenFramebuffers(1, &m_DepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Light::~Light() { }

void Light::SetStrength(const glm::vec3 strength)
{
	m_Strength = strength;

	m_IsUpdate = true;
}

void Light::SetEnabled(const bool value)
{
	m_Enabled = value;

	m_IsUpdate = true;
}

LightType Light::GetLightType() const
{
	return m_LightType;
}

bool Light::GetEnabled() const
{
	return m_Enabled;
}

bool Light::GetEnabeldLightShaft() const
{
	return m_EnabeldLightShaft;
}

void Light::SetEnabledLightShaft(const bool value)
{
	m_EnabeldLightShaft = value;
}

void Light::SetShadowMapIndex(GLuint* shadowMapArray, const int index)
{
	m_ShadowMap = shadowMapArray;
	m_ShadowMapIndex = index;
	m_LightConstants.m_LightData.m_ShadowMapIndex = index;
}