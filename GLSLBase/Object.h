#pragma once

#include "Dependencies\glew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"

class Object
{
public:
	Object();
	~Object();

public:
	virtual void Update(float deltaTime);

public:
	void Move(const float x, const float y, const float z);
	void Move(const glm::vec3& move);
	void Rotate(const float pitch, const float yaw, const float roll);
	void Rotate(const glm::vec3& rotate);
	void Scale(const float x, const float y, const float z);
	void Scale(const glm::vec3& scale);

	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const glm::vec3& position);
	void SetRotation(const float pitch, const float yaw, const float roll);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const float x, const float y, const float z);
	void SetScale(const glm::vec3& scale);

	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;
	glm::vec3 GetScale() const;

	glm::vec3 GetLook() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetRight() const;

protected:
	glm::mat4 m_World = glm::mat4();

	glm::vec3 m_Position = glm::vec3(0.0f);
	glm::vec3 m_Rotation = glm::vec3(0.0f);
	glm::vec3 m_Scale = glm::vec3(1.0f);

	bool m_IsUpdate = false;
};