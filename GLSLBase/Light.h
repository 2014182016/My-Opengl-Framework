#pragma once

#include "Object.h"
#include "UniformBuffer.hpp"

#define SHADOWMAP_SIZE 1024

enum class LightType : unsigned int
{
	Directional = 0,
	Spot,
	Point,
};

class Shader;
class Scene;

class Light : public Object
{
public:
	Light(UniformBuffer<LightConstants>* buffer);
	~Light();

public:
	// ���̴��� ���� uniform ������ ���⼭ ���´�.
	virtual void Bind() = 0;
	virtual glm::mat4 GetLightVolumeTransform() const = 0;

public:
	void SetStrength(const glm::vec3 strength);
	void SetEnabled(const bool value);
	void SetEnabledLightShaft(const bool value);
	void SetShadowMapIndex(GLuint* shadowMapArray, const int index);

	LightType GetLightType() const;
	bool GetEnabled() const;
	bool GetEnabeldLightShaft() const;

protected:
	// Viwe, Projection Tranform ������Ʈ
	virtual void UpdateTransform() = 0;
	// Render ShadowMap
	virtual void RenderShadowMap(Scene* scene) = 0;

protected:
	int m_LightIndex;
	int m_ShadowMapIndex;
	LightType m_LightType;
	LightConstants m_LightConstants;

	glm::vec3 m_Strength = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_ScatteringDensity = 10000.0f;

	bool m_Enabled = true;
	bool m_EnabeldLightShaft = false;

	// ShadowMap�� �����ϴ� �ؽ�ó
	GLuint* m_ShadowMap;
	// ShadowMap�� �׸��� ���� FBO
	GLuint m_DepthFBO;

	UniformBuffer<LightConstants>* m_LightUniformBuffer = nullptr;

private:
	static int lightNum;

};

