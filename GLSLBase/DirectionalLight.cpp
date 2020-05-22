#include "stdafx.h"
#include "DirectionalLight.h"
#include "Shader.h"
#include "Renderer.h"
#include "Debug.h"
#include "Scene.h"
#include "Camera.h"

float DirectionalLight::cascadeDists[NUM_CASCADES + 1] = { 0.0f, 25.0f, 90.0f, 0.0f };

DirectionalLight::DirectionalLight(UniformBuffer<LightConstants>* buffer) : Light(buffer)
{
	m_LightType = LightType::Directional;
	m_LightConstants.m_LightData.m_Type = (int)m_LightType;
}

DirectionalLight::~DirectionalLight() { }

void DirectionalLight::Bind()
{
	if (m_LightUniformBuffer == nullptr)
	{
		std::cout << "LightUnfiromBuffer isn't set" << std::endl;
		return;
	}

	m_LightConstants.m_LightData.m_Transform[0] = m_ViewProj[0];
	m_LightConstants.m_LightData.m_Transform[1] = m_ViewProj[1];
	m_LightConstants.m_LightData.m_Transform[2] = m_ViewProj[2];

	m_LightConstants.m_LightData.m_Strength = m_Strength;
	m_LightConstants.m_LightData.m_Direction = GetLook();
	m_LightConstants.m_LightData.m_Position = GetPosition();

	m_LightConstants.m_LightData.m_Enabled = m_Enabled;

	// Bind Uniform Buffer
	m_LightUniformBuffer->Bind(m_LightConstants);
}

void DirectionalLight::Update(float deltaTime)
{
	__super::Update(deltaTime);

	UpdateTransform();
	Bind();
	RenderShadowMap(Renderer::GetInstance()->GetCurrentScene());
}

void DirectionalLight::UpdateTransform()
{
	Camera* camera = Renderer::GetInstance()->GetCurrentScene()->GetCamera();
	glm::mat4 invCameraView = glm::inverse(camera->GetView());
	cascadeDists[0] = camera->GetNear();
	cascadeDists[NUM_CASCADES] = camera->GetFar();

	float aspect = camera->GetAspect();
	float tanHFov = tanf(glm::radians(camera->GetFov() / 2.0f));
	float tanVFov = tanf(glm::radians(camera->GetFov() * aspect / 2.0f));

	for (int i = 0; i < NUM_CASCADES; ++i)
	{
		float xn = cascadeDists[i] * tanHFov;
		float xf = cascadeDists[i + 1] * tanHFov;
		float yn = cascadeDists[i] * tanVFov;
		float yf = cascadeDists[i + 1] * tanVFov;

		glm::vec4 frustumCorners[8] = {
			// near plane
			glm::vec4(xn, yn, -cascadeDists[i], 1.0f),
			glm::vec4(-xn, yn, -cascadeDists[i], 1.0f),
			glm::vec4(-xn, -yn, -cascadeDists[i], 1.0f),
			glm::vec4(xn, -yn, -cascadeDists[i], 1.0f),

			// far plane
			glm::vec4(xf, yf, -cascadeDists[i + 1], 1.0f),
			glm::vec4(-xf, yf, -cascadeDists[i + 1], 1.0f),
			glm::vec4(-xf, -yf, -cascadeDists[i + 1], 1.0f),
			glm::vec4(xf, -yf, -cascadeDists[i + 1], 1.0f),
		};

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();

		glm::mat4 view = glm::lookAt(glm::vec3(0.0f), GetLook(), glm::vec3(0.0f, 1.0f, 0.0f));

		for (int j = 0; j < 8; ++j)
		{
			frustumCorners[j] = view * invCameraView * frustumCorners[j];

			minX = std::min(minX, frustumCorners[j].x);
			maxX = std::max(maxX, frustumCorners[j].x);
			minY = std::min(minY, frustumCorners[j].y);
			maxY = std::max(maxY, frustumCorners[j].y);
			minZ = std::min(minZ, frustumCorners[j].z);
			maxZ = std::max(maxZ, frustumCorners[j].z);
		}

		glm::mat4 proj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		m_ViewProj[i] = proj * view;
	}
}

void DirectionalLight::RenderShadowMap(Scene* scene)
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

	Shader* shader = Renderer::GetInstance()->GetShader("CascadedShadowMap");
	if (shader == nullptr)
	{
		std::cout << "CascadedShadowMap Shader isn't Find" << std::endl;
		return;
	}

	shader->Use();
	shader->SetUniform("g_World", m_World);
	shader->SetUniform("g_ViewProj", m_ViewProj[0], 3);
	shader->SetUniform("g_ShadowMapIndex", m_ShadowMapIndex);

	// Render
	scene->Render(RenderLayer::Opaque, shader);
	scene->Render(RenderLayer::AlphaTested, shader);
	scene->Render(RenderLayer::Transparent, shader);

	// Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 DirectionalLight::GetLightVolumeTransform() const
{
	glm::mat4 transform = glm::scale(glm::mat4(), glm::vec3(500.0f));

	return transform;
}