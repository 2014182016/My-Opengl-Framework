//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	SetLens(60.0f, 1.0f, 0.1f, 1000.0f);
}

Camera::~Camera() { }

void Camera::SetLens(const float fovY, const float aspect, const float zn, const float zf)
{
	m_Near = zn;
	m_Far = zf;
	m_Fov = fovY;
	m_Aspect = aspect;

	m_Proj = glm::perspective(glm::radians(fovY), aspect, zn, zf);
}

void Camera::Strafe(const float distance)
{
	glm::vec3 position = m_Position + m_Right * distance * m_CameraMoveSpeed;
	SetPosition(position);
}

void Camera::Walk(const float distance)
{
	glm::vec3 position = m_Position + m_Look * distance * m_CameraMoveSpeed;
	SetPosition(position);
}

void Camera::Pitch(const float degree)
{
	glm::mat4 rotate = glm::rotate(glm::mat4(), glm::radians(degree * m_CameraRotateSpeed), m_Right);

	m_Up = rotate * glm::vec4(m_Up, 0.0f);
	m_Look = rotate * glm::vec4(m_Look, 0.0f);

	m_ViewDirty = true;
}

void Camera::RotateY(const float degree)
{
	glm::mat4 rotate = glm::rotate(glm::mat4(), glm::radians(degree * m_CameraRotateSpeed), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Right = rotate * glm::vec4(m_Right, 0.0f);
	m_Up = rotate * glm::vec4(m_Up, 0.0f);
	m_Look = rotate * glm::vec4(m_Look, 0.0f);

	m_ViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (!m_ViewDirty)
		return;

	glm::vec3 look = glm::normalize(m_Look);
	glm::vec3 up = glm::normalize(glm::cross(m_Look, m_Right));
	glm::vec3 right = glm::cross(up, m_Look);

	float x = -glm::dot(m_Position, right);
	float y = -glm::dot(m_Position, up);
	float z = -glm::dot(m_Position, look);

	m_View[0][0] = right.x;
	m_View[1][0] = right.y;
	m_View[2][0] = right.z;
	m_View[3][0] = x;

	m_View[0][1] = up.x;
	m_View[1][1] = up.y;
	m_View[2][1] = up.z;
	m_View[3][1] = y;

	m_View[0][2] = look.x;
	m_View[1][2] = look.y;
	m_View[2][2] = look.z;
	m_View[3][2] = z;

	m_View[0][3] = 0.0f;
	m_View[1][3] = 0.0f;
	m_View[2][3] = 0.0f;
	m_View[3][3] = 1.0f;

	m_ViewDirty = false;
}

glm::vec3 Camera::GetPosition() const
{
	return m_Position;
}

void Camera::SetPosition(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;

	m_ViewDirty = true;
}

void Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;

	m_ViewDirty = true;
}

glm::mat4 Camera::GetView() const
{
	return m_View;
}

glm::mat4 Camera::GetProj() const
{
	return m_Proj;
}

glm::mat4 Camera::GetViewProj() const
{
	return m_Proj * m_View;
}

float Camera::GetNear() const
{
	return m_Near;
}

float Camera::GetFar() const
{
	return m_Far;
}

float Camera::GetFov() const
{
	return m_Fov;
}

float Camera::GetAspect() const
{
	return m_Aspect;
}