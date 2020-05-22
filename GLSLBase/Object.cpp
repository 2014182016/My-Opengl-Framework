#include "stdafx.h"
#include "Object.h"

Object::Object() { }

Object::~Object() { }

void Object::Update(float delatTime)
{
	if (!m_IsUpdate)
		return;

	m_World = glm::translate(glm::mat4(), m_Position);
	m_World = m_World * glm::eulerAngleXYZ(
		glm::radians(m_Rotation.x), glm::radians(m_Rotation.y), glm::radians(m_Rotation.z));
	m_World = glm::scale(m_World, m_Scale);

	m_IsUpdate = false;
}

void Object::Move(const float x, const float y, const float z)
{
	m_Position.x += x;
	m_Position.y += y;
	m_Position.z += z;

	m_IsUpdate = true;
}

void Object::Move(const glm::vec3& move)
{
	m_Position += move;

	m_IsUpdate = true;
}

void Object::Rotate(const float pitch, const float yaw, const float roll)
{
	m_Rotation.x += pitch;
	m_Rotation.y += yaw;
	m_Rotation.z += roll;

	m_IsUpdate = true;
}

void Object::Rotate(const glm::vec3& rotate)
{
	m_Rotation += rotate;

	m_IsUpdate = true;
}

void Object::Scale(const float x, const float y, const float z)
{
	m_Scale.x += x;
	m_Scale.y += y;
	m_Scale.z += z;

	m_IsUpdate = true;
}

void Object::Scale(const glm::vec3& scale)
{
	m_Scale += scale;

	m_IsUpdate = true;
}

void Object::SetPosition(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;

	m_IsUpdate = true;
}

void Object::SetPosition(const glm::vec3& position)
{
	m_Position = position;

	m_IsUpdate = true;
}

void Object::SetRotation(const float pitch, const float yaw, const float roll)
{
	m_Rotation.x = pitch;
	m_Rotation.y = yaw;
	m_Rotation.z = roll;

	m_IsUpdate = true;
}

void Object::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;

	m_IsUpdate = true;
}

void Object::SetScale(const float x, const float y, const float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;

	m_IsUpdate = true;
}

void Object::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;

	m_IsUpdate = true;
}

glm::vec3 Object::GetPosition() const
{
	return m_Position;
}

glm::vec3 Object::GetRotation() const
{
	return m_Rotation;
}

glm::vec3 Object::GetScale() const
{
	return m_Scale;
}

glm::vec3 Object::GetLook() const
{
	glm::vec3 look = glm::vec3(m_World[2][0], m_World[2][1], m_World[2][2]);
	return glm::normalize(look);
}

glm::vec3 Object::GetUp() const
{
	glm::vec3 look = glm::vec3(m_World[1][0], m_World[1][1], m_World[1][2]);
	return glm::normalize(look);
}

glm::vec3 Object::GetRight() const
{
	glm::vec3 look = glm::vec3(m_World[0][0], m_World[0][1], m_World[0][2]);
	return glm::normalize(look);
}