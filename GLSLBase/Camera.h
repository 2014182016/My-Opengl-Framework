//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#pragma once

#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"

class Camera
{
public:
	Camera();
	~Camera();

public:
	// fovY는 Degree 기준
	void SetLens(const float fovY, const float aspect, const float zn, const float zf);

	void Strafe(const float distance);
	void Walk(const float distance);

	void Pitch(const float degree);
	void RotateY(const float degree);

	// 카메라가 이동 및 회전을 하였다면 뷰 행렬을 업데이트한다.
	void UpdateViewMatrix();

	glm::vec3 GetPosition()const;
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const glm::vec3& position);

	glm::mat4 GetView()const;
	glm::mat4 GetProj()const;
	glm::mat4 GetViewProj()const;

	float GetNear() const;
	float GetFar() const;
	float GetFov() const;
	float GetAspect() const;

private:
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };
	glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 m_Look = { 0.0f, 0.0f, 1.0f };

	glm::mat4 m_View = glm::mat4();
	glm::mat4 m_Proj = glm::mat4();

	float m_Near;
	float m_Far;
	float m_Fov;
	float m_Aspect;

	float m_CameraMoveSpeed = 100.0f;
	float m_CameraRotateSpeed = 0.25f;

	bool m_ViewDirty = true;
};