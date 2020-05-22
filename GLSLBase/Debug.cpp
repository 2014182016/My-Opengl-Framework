#include "stdafx.h"
#include "Debug.h"

std::list<Debug::DebugObject> Debug::debugMeshes;

void Debug::Render()
{
	for (const auto& debugMesh : debugMeshes)
	{
		debugMesh.m_DebugMesh->Render();
	}
}

void Debug::Update(float deltaTime)
{
	for(auto iter = debugMeshes.begin(); iter != debugMeshes.end();)
	{
		iter->m_LifeTime -= deltaTime;
		if (iter->m_LifeTime <= 0.0f)
		{
			iter = debugMeshes.erase(iter);
			continue;
		}

		++iter;
	}
}

void Debug::DrawLine(const glm::vec3& p1, const glm::vec3& p2, const float time, const glm::vec3& color)
{
	std::vector<LineVertex> vertices;

	vertices.push_back(LineVertex(p1, color));
	vertices.push_back(LineVertex(p2, color));

	CreateDebugMesh(&vertices, nullptr, GL_LINES, time);
}

void Debug::DrawCube(const glm::vec3& center, const float width, const float height, const float depth, 
	const float time, const glm::vec3& color)
{
	std::vector<LineVertex> vertices;

	vertices.push_back(LineVertex(glm::vec3(center.x - width, center.y - height, center.z - depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x + width, center.y - height, center.z - depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x + width, center.y - height, center.z + depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x - width, center.y - height, center.z + depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x - width, center.y + height, center.z - depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x + width, center.y + height, center.z - depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x + width, center.y + height, center.z + depth), color));
	vertices.push_back(LineVertex(glm::vec3(center.x - width, center.y + height, center.z + depth), color));

	std::vector<uint16_t> indices = 
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};

	CreateDebugMesh(&vertices, &indices, GL_LINES, time);
}

void Debug::DrawCube(const glm::vec3& center, const float width, const float height, const float depth, const glm::vec3& rotation,
	const float time, const glm::vec3& color)
{
	std::vector<LineVertex> vertices;

	glm::mat4 transform = glm::translate(glm::mat4(), center) * glm::eulerAngleXYZ(
		glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));

	glm::vec4 p0 = transform * glm::vec4(-width, -height, -depth, 1.0f);
	glm::vec4 p1 = transform * glm::vec4(+width, -height, -depth, 1.0f);
	glm::vec4 p2 = transform * glm::vec4(+width, -height, +depth, 1.0f);
	glm::vec4 p3 = transform * glm::vec4(-width, -height, +depth, 1.0f);
	glm::vec4 p4 = transform * glm::vec4(-width, +height, -depth, 1.0f);
	glm::vec4 p5 = transform * glm::vec4(+width, +height, -depth, 1.0f);
	glm::vec4 p6 = transform * glm::vec4(+width, +height, +depth, 1.0f);
	glm::vec4 p7 = transform * glm::vec4(-width, +height, +depth, 1.0f);

	vertices.push_back(LineVertex(glm::vec3(p0), color));
	vertices.push_back(LineVertex(glm::vec3(p1), color));
	vertices.push_back(LineVertex(glm::vec3(p2), color));
	vertices.push_back(LineVertex(glm::vec3(p3), color));
	vertices.push_back(LineVertex(glm::vec3(p4), color));
	vertices.push_back(LineVertex(glm::vec3(p5), color));
	vertices.push_back(LineVertex(glm::vec3(p6), color));
	vertices.push_back(LineVertex(glm::vec3(p7), color));

	std::vector<uint16_t> indices =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};

	CreateDebugMesh(&vertices, &indices, GL_LINES, time);
}

void Debug::DrawSphere(const glm::vec3& center, const float radius, const float time, const glm::vec3& color)
{
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f) * radius;
	glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f)  * radius;
	glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f)  * radius;

	DrawRing(center, xAxis, zAxis, time, color);
	DrawRing(center, xAxis, yAxis, time, color);
	DrawRing(center, yAxis, zAxis, time, color);
}

void Debug::DrawRing(const glm::vec3& center, const glm::vec3& majorAxis, const glm::vec3& minorAxis,
	const float time, const glm::vec3& color)
{
	static const int ringSegments = 32;
	static constexpr float angleDelta = glm::two_pi<float>() / ringSegments;

	std::vector<LineVertex> vertices;

	glm::vec3 cosDelta = glm::vec3(cosf(angleDelta));
	glm::vec3 sinDelta = glm::vec3(sinf(angleDelta));
	glm::vec3 incrementalSin = glm::vec3(0.0f);
	glm::vec3 incrementalCos = glm::vec3(1.0f);

	for (int i = 0; i < ringSegments + 1; ++i)
	{
		// 새로운 위치를 계산한다.
		glm::vec3 pos = (majorAxis * incrementalCos + minorAxis * incrementalSin) + center;

		// 위치와 색을 저장한다.
		vertices.push_back(LineVertex(pos, color));

		// 새로운 증가량을 계산한다.
		glm::vec3 newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		glm::vec3 newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}

	CreateDebugMesh(&vertices, nullptr, GL_LINE_STRIP, time);
}

void Debug::DrawTetrahedron(const glm::vec3& near0, const glm::vec3& far0, const glm::vec3& far1,
	const glm::vec3& far2, const glm::vec3& far3, const float time, const glm::vec3& color)
{
	std::vector<LineVertex> vertices;

	vertices.push_back(LineVertex(near0, color));
	vertices.push_back(LineVertex(far0, color));
	vertices.push_back(LineVertex(near0, color));
	vertices.push_back(LineVertex(far1, color));
	vertices.push_back(LineVertex(near0, color));
	vertices.push_back(LineVertex(far2, color));
	vertices.push_back(LineVertex(near0, color));
	vertices.push_back(LineVertex(far3, color));

	vertices.push_back(LineVertex(far0, color));
	vertices.push_back(LineVertex(far1, color));
	vertices.push_back(LineVertex(far1, color));
	vertices.push_back(LineVertex(far2, color));
	vertices.push_back(LineVertex(far2, color));
	vertices.push_back(LineVertex(far3, color));
	vertices.push_back(LineVertex(far3, color));
	vertices.push_back(LineVertex(far0, color));

	CreateDebugMesh(&vertices, nullptr, GL_LINES, time);
}

void Debug::DrawFrustum(const glm::vec3* pos, const float time, const glm::vec3& color)
{
	std::vector<LineVertex> vertices;

	vertices.push_back(LineVertex(pos[0], color));
	vertices.push_back(LineVertex(pos[1], color));
	vertices.push_back(LineVertex(pos[1], color));
	vertices.push_back(LineVertex(pos[2], color));
	vertices.push_back(LineVertex(pos[2], color));
	vertices.push_back(LineVertex(pos[3], color));
	vertices.push_back(LineVertex(pos[3], color));
	vertices.push_back(LineVertex(pos[0], color));

	vertices.push_back(LineVertex(pos[4], color));
	vertices.push_back(LineVertex(pos[5], color));
	vertices.push_back(LineVertex(pos[5], color));
	vertices.push_back(LineVertex(pos[6], color));
	vertices.push_back(LineVertex(pos[6], color));
	vertices.push_back(LineVertex(pos[7], color));
	vertices.push_back(LineVertex(pos[7], color));
	vertices.push_back(LineVertex(pos[4], color));

	vertices.push_back(LineVertex(pos[0], color));
	vertices.push_back(LineVertex(pos[4], color));
	vertices.push_back(LineVertex(pos[1], color));
	vertices.push_back(LineVertex(pos[5], color));
	vertices.push_back(LineVertex(pos[2], color));
	vertices.push_back(LineVertex(pos[6], color));
	vertices.push_back(LineVertex(pos[3], color));
	vertices.push_back(LineVertex(pos[7], color));

	CreateDebugMesh(&vertices, nullptr, GL_LINES, time);
}

void Debug::CreateDebugMesh(std::vector<LineVertex>* vertices, std::vector<uint16_t>* indices, const GLenum primitive, const float time)
{
	std::unique_ptr<StaticMesh> debugMesh = std::make_unique<StaticMesh>();

	debugMesh->CreateBufferObjects();
	debugMesh->CreateVertexObject((*vertices).data(), (GLuint)(*vertices).size(), sizeof(LineVertex));
	if(indices != nullptr)
		debugMesh->CreateElementObject((*indices).data(), (GLuint)(*indices).size(), sizeof(uint16_t));
	LineVertex::SetVertexAttribute();

	debugMesh->SetPrimitive(primitive);

	DebugObject debugObject;
	debugObject.m_DebugMesh = std::move(debugMesh);
	debugObject.m_LifeTime = time;

	debugMeshes.push_back(std::move(debugObject));
}