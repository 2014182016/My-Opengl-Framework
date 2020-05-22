#pragma once

#include "Dependencies\glew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"

template<typename Constants>
class UniformBuffer
{
public:
	UniformBuffer(GLuint bindingPoint, GLsizeiptr constantsNum = 1, GLenum usage = GL_STATIC_DRAW)
	{
		// Uniform Buffer Object를 생성
		glGenBuffers(1, &m_UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
		// 버퍼 메모리 할당
		glBufferData(GL_UNIFORM_BUFFER, sizeof(Constants) * constantsNum, NULL, usage);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_UBO);
		// Unbind
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	~UniformBuffer()
	{
		glDeleteBuffers(1, &m_UBO);
	}

public:
	void Bind(const Constants& constatns) const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
		constatns.Bind();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

private:
	GLuint m_UBO;

};

class Constants
{
public:
	virtual void Bind() const = 0;
};

class PassConstants : public Constants
{
public:
	PassConstants() = default;
	~PassConstants() = default;

public:
	virtual void Bind() const override
	{
		GLintptr offset = 0;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &m_View[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::mat4), &m_InvView[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::mat4), &m_Proj[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::mat4), &m_InvProj[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::mat4), &m_ViewProj[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::mat4), &m_InvViewProj[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4), sizeof(glm::vec4), &m_Ambient[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec4), sizeof(glm::vec3), &m_EyePos[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec3), sizeof(float), &m_DelatTime);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(glm::vec2), &m_ScreenDimension);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec2), sizeof(glm::vec2), &m_InvScreenDimension);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec2), sizeof(glm::vec2), &m_InvHalfScreenDimension);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec2), sizeof(float), &m_Near);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_Far);
	}

public:
	glm::mat4 m_View;
	glm::mat4 m_InvView;
	glm::mat4 m_Proj;
	glm::mat4 m_InvProj;
	glm::mat4 m_ViewProj;
	glm::mat4 m_InvViewProj;
	glm::vec4 m_Ambient;
	glm::vec3 m_EyePos;
	float m_DelatTime;
	glm::vec2 m_ScreenDimension;
	glm::vec2 m_InvScreenDimension;
	glm::vec2 m_InvHalfScreenDimension;
	float m_Near;
	float m_Far;
};

class LightConstants : public Constants
{
public:
	struct LightData
	{
		glm::mat4 m_Transform[3];

		glm::vec3 m_Strength;
		float m_FalloffStart;

		glm::vec3 m_Direction;
		float m_FalloffEnd;

		glm::vec3 m_Position;
		float m_SpotAngle;

		int m_Type;
		int m_LightIndex;
		int m_ShadowMapIndex;
		bool m_Enabled;
	};

public:
	LightConstants() = default;
	~LightConstants() = default;

public:
	virtual void Bind() const override
	{
		static constexpr GLsizeiptr dataSize = sizeof(LightData);
		GLintptr offset = dataSize * m_LightData.m_LightIndex;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4) * 3, &m_LightData.m_Transform[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::mat4) * 3, sizeof(glm::vec3), &m_LightData.m_Strength[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec3), sizeof(float), &m_LightData.m_FalloffStart);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(glm::vec3), &m_LightData.m_Direction[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec3), sizeof(float), &m_LightData.m_FalloffEnd);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(glm::vec3), &m_LightData.m_Position[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(glm::vec3), sizeof(float), &m_LightData.m_SpotAngle);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(int), &m_LightData.m_Type);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(int), sizeof(int), &m_LightData.m_LightIndex);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(int), sizeof(int), &m_LightData.m_ShadowMapIndex);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(int), sizeof(bool), &m_LightData.m_Enabled);
	}

public:
	LightData m_LightData;

};

class PostProcessConstants : public Constants
{
public:
	PostProcessConstants() = default;
	~PostProcessConstants() = default;

public:
	virtual void Bind() const override
	{
		GLintptr offset = 0;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &m_InvGamma);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_Exposure);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_ScatteringTau);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_ScatteringDensity);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_ScatteringZFar);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &m_Anisotropy);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_Decay);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_Density);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_Density);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_Weight);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_ScatteringExposure);
		glBufferSubData(GL_UNIFORM_BUFFER, offset += sizeof(float), sizeof(float), &g_MaxTexDistance);
	}

public:
	float m_InvGamma = 1.0f / 2.2f; // Gamma Correction
	float m_Exposure = 1.0f; // Gamma Correction
	float m_ScatteringTau = 0.03f; // Volumetric Light Scattering
	float m_ScatteringDensity = 100000.0f; // Volumetric Light Scattering
	float m_ScatteringZFar = 500.0f; // Volumetric Light Scattering
	float m_Anisotropy = 0.01f; // Volumetric Light Scattering
	float g_Decay = 0.85f; // Radial Blur
	float g_Density = 0.75f; // Radial Blur
	float g_Weight = 3.5f; // Radial Blur
	float g_ScatteringExposure = 100.0f; // Radial Blur
	float g_MaxTexDistance = 1.0f; // Radial Blur
};
