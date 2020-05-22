#include "stdafx.h"
#include "PointLight.h"
#include "Shader.h"
#include "Debug.h"
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"

PointLight::PointLight(UniformBuffer<LightConstants>* buffer) : Light(buffer)
{
	m_LightType = LightType::Point;
	m_LightConstants.m_LightData.m_Type = (int)m_LightType;
}

PointLight::~PointLight() { }


void PointLight::Bind()
{
	if (m_LightUniformBuffer == nullptr)
	{
		std::cout << "LightUnfiromBuffer isn't set" << std::endl;
		return;
	}

	m_LightConstants.m_LightData.m_Enabled = m_Enabled;

	m_LightConstants.m_LightData.m_Strength = m_Strength;
	m_LightConstants.m_LightData.m_Direction = GetLook();
	m_LightConstants.m_LightData.m_Position = GetPosition();

	m_LightConstants.m_LightData.m_FalloffStart = m_FalloffStart;
	m_LightConstants.m_LightData.m_FalloffEnd = m_FalloffEnd;

	m_LightUniformBuffer->Bind(m_LightConstants);
}

void PointLight::SetFalloffStart(const float falloffStart)
{
	m_FalloffStart = falloffStart;

	m_IsUpdate = true;
}

void PointLight::SetFalloffEnd(const float falloffEnd)
{
	m_FalloffEnd = falloffEnd;

	m_IsUpdate = true;
}

void PointLight::Update(float deltaTime)
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


void PointLight::UpdateTransform()
{
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, m_FalloffEnd);
	m_ViewProj[0] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_ViewProj[1] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_ViewProj[2] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_ViewProj[3] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	m_ViewProj[4] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_ViewProj[5] = proj * glm::lookAt(GetPosition(), GetPosition() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void PointLight::RenderShadowMap(Scene* scene)
{
	if (scene == nullptr)
		return;

	// Setting
	glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (*m_ShadowMap), 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	Shader* shader = Renderer::GetInstance()->GetShader("OmnidirectionalShadowMap");
	if (shader == nullptr)
	{
		std::cout << "OmnidirectionalShadowMap Shader isn't Find" << std::endl;
		return;
	}

	shader->Use();
	shader->SetUniform("g_World", m_World);
	shader->SetUniform("g_ViewProj", m_ViewProj[0], 6);
	shader->SetUniform("g_LightPos", GetPosition());
	shader->SetUniform("g_ShadowMapIndex", m_ShadowMapIndex);
	shader->SetUniform("g_Far", scene->GetCamera()->GetFar());

	// Render
	scene->Render(RenderLayer::Opaque, shader);
	scene->Render(RenderLayer::AlphaTested, shader);
	scene->Render(RenderLayer::Transparent, shader);

	// Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 PointLight::GetLightVolumeTransform() const
{
	glm::mat4 transform = glm::translate(glm::mat4(), m_Position);
	transform = glm::scale(transform, glm::vec3(m_FalloffEnd * 2.0f));

	return transform;
}