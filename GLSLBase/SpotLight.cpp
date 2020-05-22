#include "stdafx.h"
#include "SpotLight.h"
#include "Shader.h"
#include "Debug.h"
#include "Scene.h"
#include "Renderer.h"

SpotLight::SpotLight(UniformBuffer<LightConstants>* buffer) : Light(buffer)
{
	m_LightType = LightType::Spot;
	m_LightConstants.m_LightData.m_Type = (int)m_LightType;
}

SpotLight::~SpotLight() { }

void SpotLight::Bind()
{
	if (m_LightUniformBuffer == nullptr)
	{
		std::cout << "LightUnfiromBuffer isn't set" << std::endl;
		return;
	}

	m_LightConstants.m_LightData.m_Transform[0] = m_ViewProj;

	m_LightConstants.m_LightData.m_Strength = m_Strength;
	m_LightConstants.m_LightData.m_Direction = GetLook();
	m_LightConstants.m_LightData.m_Position = GetPosition();

	m_LightConstants.m_LightData.m_FalloffStart = m_FalloffStart;
	m_LightConstants.m_LightData.m_FalloffEnd = m_FalloffEnd;
	m_LightConstants.m_LightData.m_SpotAngle = m_SpotAngle;

	m_LightConstants.m_LightData.m_Enabled = m_Enabled;

	m_LightUniformBuffer->Bind(m_LightConstants);
}

void SpotLight::SetFalloffStart(const float falloffStart)
{
	m_FalloffStart = falloffStart;

	m_IsUpdate = true;
}

void SpotLight::SetFalloffEnd(const float falloffEnd)
{
	m_FalloffEnd = falloffEnd;

	m_IsUpdate = true;
}

void SpotLight::SetSpotAngle(const float spotAngle)
{
	m_SpotAngle = spotAngle;

	m_IsUpdate = true;
}

void SpotLight::Update(float deltaTime)
{
	if (m_IsUpdate)
	{
		m_World = glm::translate(glm::mat4(), m_Position);
		m_World = m_World * glm::eulerAngleXYZ(
			glm::radians(m_Rotation.x), glm::radians(m_Rotation.y), glm::radians(m_Rotation.z));
		m_World = glm::scale(m_World, m_Scale);

		// 업데이트를 할때만 수행한다.
		UpdateTransform();
		Bind();

		m_IsUpdate = false;
	}

	RenderShadowMap(Renderer::GetInstance()->GetCurrentScene());
}

void SpotLight::UpdateTransform()
{
	glm::mat4 view = glm::lookAt(GetPosition(), GetPosition() + GetLook(), GetUp());
	glm::mat4 proj = glm::perspective(glm::radians(m_SpotAngle * 1.5f), 1.0f, m_FalloffStart, m_FalloffEnd);
	m_ViewProj = proj * view;
}

void SpotLight::RenderShadowMap(Scene* scene)
{
	if (scene == nullptr)
		return;

	// Setting
	glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (*m_ShadowMap), 0, m_ShadowMapIndex);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	Shader* shader = Renderer::GetInstance()->GetShader("ShadowMap");
	if (shader == nullptr)
	{
		std::cout << "ShadowMap Shader isn't Find" << std::endl;
		return;
	}

	shader->Use();
	shader->SetUniform("g_World", m_World);
	shader->SetUniform("g_ViewProj", m_ViewProj);

	// Render
	scene->Render(RenderLayer::Opaque, shader);
	scene->Render(RenderLayer::AlphaTested, shader);
	scene->Render(RenderLayer::Transparent, shader);

	// Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 SpotLight::GetLightVolumeTransform() const
{
	glm::mat4 transform = glm::translate(glm::mat4(), m_Position);
	transform = transform * glm::eulerAngleXYZ(
		glm::radians(m_Rotation.x), glm::radians(m_Rotation.y), glm::radians(m_Rotation.z));
	transform = glm::scale(transform, glm::vec3(m_SpotAngle / 2.0f, m_SpotAngle / 2.0f, m_FalloffEnd));

	return transform;
}