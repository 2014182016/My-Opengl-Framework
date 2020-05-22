#pragma once

#include "Light.h"

#define NUM_CASCADES 3

class Shader;
class Scene;

class DirectionalLight : public Light
{
public:
	DirectionalLight(UniformBuffer<LightConstants>* buffer);
	~DirectionalLight();

public:
	virtual void Update(float deltaTime) override;
	virtual void Bind() override; // 쉐이더에 묶을 uniform 변수를 여기서 묶는다.
	virtual glm::mat4 GetLightVolumeTransform() const override;

protected:
	// Viwe, Projection Tranform 업데이트
	virtual void UpdateTransform() override;
	// Render ShadowMap
	virtual void RenderShadowMap(Scene* scene) override;

public:
	static float cascadeDists[NUM_CASCADES + 1];

protected:
	glm::mat4 m_ViewProj[NUM_CASCADES];

};

