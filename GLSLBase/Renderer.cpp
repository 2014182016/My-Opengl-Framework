#include "stdafx.h"
#include "Renderer.h"
#include "Debug.h"
#include "Scene.h"
#include "Shader.h"
#include "BlurFilter.h"
#include "Light.h"
#include "Camera.h"

Renderer* Renderer::instance = nullptr;
int Renderer::m_ScreenWidth = 0;
int Renderer::m_ScreenHeight = 0;
const GLuint Renderer::attachments_FourColorBuffer[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
const GLuint Renderer::attachments_ThreeColorBuffer[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
const GLuint Renderer::attachments_TwoColorBuffer[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
const GLuint Renderer::attachments_OneColorBuffer[1] = { GL_COLOR_ATTACHMENT0 };

Renderer::Renderer() 
{
	assert(instance == nullptr);
	instance = this;
}

Renderer::~Renderer()
{
	m_Shaders.clear();

	glDeleteTextures(1, &m_DepthBuffer);
	glDeleteTextures(1, &m_SceneBuffer);
	glDeleteTextures(1, &m_BloomBuffer);
	glDeleteTextures(1, &m_LightIndexedBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_DefaultFBO);
}

Renderer* Renderer::GetInstance()
{
	if (instance == nullptr)
		instance = new Renderer();
	return instance;
}

glm::vec2 Renderer::GetScreenDimension()
{
	return glm::vec2(m_ScreenWidth, m_ScreenHeight);
}

Shader* Renderer::GetShader(const char* name)
{
	return m_Shaders[name].get();
}

Scene* Renderer::GetCurrentScene()
{
	return m_Scene;
}

void Renderer::Initialize(const int windowWidth, const int windowHeight, Scene* scene)
{
	assert(scene);
	m_Scene = scene;
	scene->Initialize(windowWidth, windowHeight);

	m_ScreenWidth = windowWidth;
	m_ScreenHeight = windowHeight;

	CreateShaders();
	CreateFrameBuffers(windowWidth, windowHeight);

	m_PostProcessUniformBuffer = std::make_unique<UniformBuffer<PostProcessConstants>>(2, 1, GL_STATIC_DRAW);
	m_BloomBlurFilter = std::make_unique<BlurFilter>(m_ScreenWidth / 2, m_ScreenHeight / 2);
	m_LightScatteringBlurFilter = std::make_unique<BlurFilter>(m_ScreenWidth / 2, m_ScreenHeight / 2);
}

void Renderer::CreateShaders()
{
	std::unique_ptr<Shader> shader;

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Forward.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "Forward.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["Forward"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Forward.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "Opaque.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["Opaque"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Line.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "Line.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["Line"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Fullscreen.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "PostProcess.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["PostProcess"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Fullscreen.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "Blur.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["Blur"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "NormalVisualization.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "NormalVisualization.fs");
	shader->AddShader(GL_GEOMETRY_SHADER, "NormalVisualization.gs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["NormalVisualization"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "ShadowMap.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "ShadowMap.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["ShadowMap"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "OmnidirectionalShadowMap.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "OmnidirectionalShadowMap.fs");
	shader->AddShader(GL_GEOMETRY_SHADER, "OmnidirectionalShadowMap.gs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["OmnidirectionalShadowMap"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "CascadedShadowMap.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "CascadedShadowMap.fs");
	shader->AddShader(GL_GEOMETRY_SHADER, "CascadedShadowMap.gs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["CascadedShadowMap"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "LightVolume.vs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["LightVolume"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Fullscreen.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "LightVolume.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["LightIndex"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Fullscreen.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "VolumetricLightScattering.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["VolumetricLightScattering"] = std::move(shader);

	shader = std::make_unique<Shader>();
	shader->AddShader(GL_VERTEX_SHADER, "Fullscreen.vs");
	shader->AddShader(GL_FRAGMENT_SHADER, "RadialBlur.fs");
	shader->Link(); // 추가한 쉐이더들을 프로그램에 링크한다.
	m_Shaders["RadialBlur"] = std::move(shader);
}

void Renderer::CreateFrameBuffers(const int windowWidth, const int windowHeight)
{
	// Depth 버퍼 생성
	glGenTextures(1, &m_DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Scene 버퍼 생성
	glGenTextures(1, &m_SceneBuffer);
	glBindTexture(GL_TEXTURE_2D, m_SceneBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bloom 버퍼 생성
	glGenTextures(1, &m_BloomBuffer);
	glBindTexture(GL_TEXTURE_2D, m_BloomBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// LightIndex 버퍼 생성
	glGenTextures(1, &m_LightIndexedBuffer);
	glBindTexture(GL_TEXTURE_2D, m_LightIndexedBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// 프레임 버퍼 생성
	glGenFramebuffers(1, &m_DefaultFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DefaultFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// Depth 버퍼 생성
	glGenTextures(1, &m_LightSctteringDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_LightSctteringDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth / 2, windowHeight / 2, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// LightSctteringBuffer 버퍼 생성
	glGenTextures(1, &m_LightSctteringBuffer);
	glBindTexture(GL_TEXTURE_2D, m_LightSctteringBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth / 2, windowHeight / 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// m_LightRadialBlurBuffer 버퍼 생성
	glGenTextures(1, &m_LightRadialBlurBuffer);
	glBindTexture(GL_TEXTURE_2D, m_LightRadialBlurBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth / 2, windowHeight / 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 프레임 버퍼 생성
	glGenFramebuffers(1, &m_LightScatteringFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_LightScatteringFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_LightSctteringDepthBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// 언바인딩 = 디폴트 프레임 버퍼로 변경
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Update(float deltaTime)
{
	m_Scene->Update(deltaTime);

#if defined(DEBUG) || defined(_DEBUG)
	Debug::Update(deltaTime);
#endif

	if (m_UpdateSetting)
	{
		m_PostProcessUniformBuffer->Bind(m_PostProcessConstants);
	}
}

void Renderer::Render()
{
	Shader* currShader = nullptr;

	// Opaque Rendering
	{
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, m_DefaultFBO);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		currShader = m_Shaders["Opaque"].get();
		currShader->Use();

		// 불투명 렌더
		glEnable(GL_CULL_FACE);
		m_Scene->Render(RenderLayer::Opaque, currShader);

		// 알파 처리 렌더
		glDisable(GL_CULL_FACE);
		m_Scene->Render(RenderLayer::AlphaTested, currShader);
	}

	// Light Volume Pass
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LightIndexedBuffer, 0);
		glDrawBuffers(1, attachments_OneColorBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		std::vector<Light*> lights;
		m_Scene->GetLights(lights);

		Shader* lightVolumeShader = m_Shaders["LightVolume"].get();
		Shader* lightIndexShader = m_Shaders["LightIndex"].get();

		for (int i = 0; i < (int)lights.size(); ++i)
		{
			if (lights[i]->GetEnabled() == false)
				continue;

			glClear(GL_STENCIL_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glStencilFunc(GL_ALWAYS, 0, 0);

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			currShader = lightVolumeShader;
			currShader->Use();
			m_Scene->RenderLightVolume(currShader, lights[i]);

			glDisable(GL_DEPTH_TEST);
			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

			glDrawBuffers(1, attachments_OneColorBuffer);

			currShader = lightIndexShader;
			currShader->Use();
			currShader->SetUniform("g_LightIndex", i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_STENCIL_TEST);
	}

	// Forward Rendering
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SceneBuffer, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BloomBuffer, 0);
		glDrawBuffers(2, attachments_TwoColorBuffer);

		byte clearColor[] = { 178, 122, 255, 0 };
		glClearTexImage(m_SceneBuffer, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearColor);
		glClearTexImage(m_BloomBuffer, 0, GL_RGBA, GL_FLOAT, NULL);

		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		currShader = m_Shaders["Opaque"].get();
		currShader->Use();

		m_Scene->BindShadowMapArray(currShader);
		currShader->BindTexture("g_LightIndexedBuffer", 15, m_LightIndexedBuffer);

		// 불투명 렌더
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		m_Scene->Render(RenderLayer::Opaque, currShader);

		// 알파 처리 렌더
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		m_Scene->Render(RenderLayer::AlphaTested, currShader);

		currShader = m_Shaders["Forward"].get();
		currShader->Use();

		// 투명 렌더
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_Scene->Render(RenderLayer::Transparent, currShader);

#if defined(DEBUG) || defined(_DEBUG)
		// 디버그 렌더
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		m_Shaders["Line"]->Use();
		Debug::Render();
#endif
	}

	//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_DefaultFBO);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_LightScatteringFBO);
	//glBlitFramebuffer(0, 0, m_ScreenWidth, m_ScreenHeight, 0, 0, m_ScreenWidth / 2, m_ScreenHeight / 2, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	// Volumetric Light Scattering Ray Marching
	{
		glViewport(0, 0, m_ScreenWidth / 2, m_ScreenHeight / 2);
		glBindFramebuffer(GL_FRAMEBUFFER, m_LightScatteringFBO);
		glDrawBuffers(1, attachments_OneColorBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_STENCIL_TEST);
		glCullFace(GL_FRONT);
		glDisable(GL_DEPTH_TEST);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_INCR);

		std::vector<Light*> lights;
		m_Scene->GetLights(lights);

		Shader* lightVolumeShader = m_Shaders["LightVolume"].get();
		Shader* lightScatteringShader = m_Shaders["VolumetricLightScattering"].get();
		Shader* radialBlurShader = m_Shaders["RadialBlur"].get();
		glm::mat4 viewProj = m_Scene->GetCamera()->GetViewProj();

		for (int i = 0; i < (int)lights.size(); ++i)
		{
			if (!lights[i]->GetEnabled() || !lights[i]->GetEnabeldLightShaft())
				continue;

			glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			currShader = lightVolumeShader;
			currShader->Use();
			m_Scene->RenderLightVolume(currShader, lights[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LightSctteringBuffer, 0);
			glDrawBuffers(1, attachments_OneColorBuffer);

			glClear(GL_COLOR_BUFFER_BIT);

			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

			currShader = lightScatteringShader;
			currShader->Use();
			currShader->BindTexture("g_DepthTexture", 0, m_DepthBuffer);
			m_Scene->BindShadowMapArray(currShader);
			currShader->SetUniform("g_LightIndex", i);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LightRadialBlurBuffer, 0);
			glDrawBuffers(1, attachments_OneColorBuffer);

			glEnable(GL_BLEND);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);

			currShader = radialBlurShader;
			currShader->Use();
			glm::vec4 lightPosH = viewProj * glm::vec4(lights[i]->GetPosition(), 1.0f);
			lightPosH /= lightPosH.w;
			lightPosH = lightPosH * 0.5f + 0.5f;
			currShader->SetUniform("g_LightPosH", glm::vec2(lightPosH.x, lightPosH.y));
			currShader->BindTexture("g_LightScatteringTexture", 0, m_LightSctteringBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_STENCIL_TEST);
	}

	// Blur
	{
		m_BloomBlurFilter->Execute(m_BloomBuffer, 2);
		m_LightScatteringBlurFilter->Execute(m_LightRadialBlurBuffer, 3);
	}

	// PostProcess
	{
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		currShader = m_Shaders["PostProcess"].get();
		currShader->Use();
		currShader->BindTexture("g_SceneTexture", 0, m_SceneBuffer);
		currShader->BindTexture("g_BloomTexture", 1, m_BloomBlurFilter->GetOutput());
		currShader->BindTexture("g_LightScatteringTexture", 2, m_LightScatteringBlurFilter->GetOutput());

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


void Renderer::SetGamma(const float gamma)
{
	m_PostProcessConstants.m_InvGamma = 1.0f / gamma;

	m_UpdateSetting = true;
}

void Renderer::SetExposure(const float exposure)
{
	m_PostProcessConstants.m_Exposure = exposure;

	m_UpdateSetting = true;
}