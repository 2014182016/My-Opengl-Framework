#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(UniformBuffer<LightConstants>* buffer);
	~PointLight();

public:
	virtual void Update(float deltaTime) override;
	virtual void Bind() override; // 쉐이더에 묶을 uniform 변수를 여기서 묶는다.
	virtual glm::mat4 GetLightVolumeTransform() const override;

public:
	void SetFalloffStart(const float falloffStart);
	void SetFalloffEnd(const float falloffEnd);

protected:
	// Viwe, Projection Tranform 업데이트
	virtual void UpdateTransform() override;
	// Render ShadowMap
	virtual void RenderShadowMap(Scene* scene) override;

protected:
	float m_FalloffStart = 1.0f;
	float m_FalloffEnd = 10.0f;

	glm::mat4 m_ViewProj[6];

};

