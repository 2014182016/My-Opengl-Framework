#pragma once

#include <unordered_map>
#include <array>

#include "Dependencies\glew.h"

#include "UniformBuffer.hpp"

class Scene;
class Shader;
class BlurFilter;

class Renderer
{
public:
	Renderer();
	Renderer& operator=(const Renderer& rhs) = delete;
	~Renderer();

public:
	static Renderer* GetInstance();
	static glm::vec2 GetScreenDimension();

public:
	void Initialize(const int windowWidth, const int windowHeight, Scene* scene);
	void Update(float deltaTime);
	void Render();

	Shader* GetShader(const char* name);
	Scene* GetCurrentScene();

	void SetGamma(const float gamma);
	void SetExposure(const float exposure);

private:
	void CreateShaders();
	void CreateFrameBuffers(const int windowWidth, const int windowHeight);

public:
	static const GLuint attachments_OneColorBuffer[1];
	static const GLuint attachments_TwoColorBuffer[2];
	static const GLuint attachments_ThreeColorBuffer[3];
	static const GLuint attachments_FourColorBuffer[4];

private:
	static Renderer* instance;

	static int m_ScreenWidth;
	static int m_ScreenHeight;

	GLuint m_DefaultFBO;
	GLuint m_DepthBuffer;
	GLuint m_SceneBuffer;
	GLuint m_BloomBuffer;
	GLuint m_LightIndexedBuffer;

	GLuint m_LightScatteringFBO;
	GLuint m_LightSctteringBuffer;
	GLuint m_LightRadialBlurBuffer;
	GLuint m_LightSctteringDepthBuffer;

	Scene* m_Scene = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;

	std::unique_ptr<UniformBuffer<PostProcessConstants>> m_PostProcessUniformBuffer = nullptr;
	PostProcessConstants m_PostProcessConstants;
	bool m_UpdateSetting = true;

	std::unique_ptr<BlurFilter> m_BloomBlurFilter;
	std::unique_ptr<BlurFilter> m_LightScatteringBlurFilter;
};

