#pragma once
#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight(UniformBuffer<LightConstants>* buffer);
	~SpotLight();

public:
	virtual void Update(float deltaTime) override;
	virtual void Bind() override; // ���̴��� ���� uniform ������ ���⼭ ���´�.
	virtual glm::mat4 GetLightVolumeTransform() const override;

public:
	void SetFalloffStart(const float falloffStart);
	void SetFalloffEnd(const float falloffEnd);
	void SetSpotAngle(const float spotAngle);

protected:
	// Viwe, Projection Tranform ������Ʈ
	virtual void UpdateTransform() override;
	// Render ShadowMap
	virtual void RenderShadowMap(Scene* scene) override;

protected:
	float m_FalloffStart = 1.0f;
	float m_FalloffEnd = 10.0f;
	float m_SpotAngle = 45.0f;

	glm::mat4 m_ViewProj;

};

