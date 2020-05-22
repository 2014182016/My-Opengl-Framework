#pragma once

#include <list>

#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "StaticMesh.h"

struct LineVertex;

class Debug
{
public:
	struct DebugObject
	{
		DebugObject() = default;

		std::unique_ptr<StaticMesh> m_DebugMesh = nullptr;
		float m_LifeTime = 0.0f;
	};

public:
	Debug() = delete;
	~Debug() = delete;

public:
	static void Render();
	static void Update(float deltaTime);

	static void DrawLine(const glm::vec3& p1, const glm::vec3& p2,
		const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawCube(const glm::vec3& center, const float width, const float height, const float depth,
		const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawCube(const glm::vec3& center, const float width, const float height, const float depth, const glm::vec3& rotation,
		const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawSphere(const glm::vec3& center, const float radius,
		const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawRing(const glm::vec3& center, const glm::vec3& majorAxis, const glm::vec3& minorAxis,
		const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawTetrahedron(const glm::vec3& near0, const glm::vec3& far0, const glm::vec3& far1,
		const glm::vec3& far2, const glm::vec3& far3, const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
	static void DrawFrustum(const glm::vec3* pos, const float time = 5.0f, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));

private:
	static void CreateDebugMesh(std::vector<LineVertex>* vertices, std::vector<uint16_t>* indices, const GLenum primitive, const float time);

private:
	static std::list<DebugObject> debugMeshes;

};