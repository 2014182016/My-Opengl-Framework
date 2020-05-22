#pragma once

#include <vector>

#include "Dependencies\glew.h"
#include "Dependencies\glm/glm.hpp"

static const std::string modelPath = "Models/";

struct Vertex;
struct aiNode;
struct aiMesh;
struct aiScene;

class StaticMesh
{
public:
	// ������ �ҷ����� ���� ��� ���ؽ� �� �ε����� ���� �����Ѵ�.
	StaticMesh();
	// ������ �ҷ��� ��� Create�Լ��� �ҷ��� �ʿ�� ����.
	StaticMesh(const char* fileName);
	~StaticMesh();

public:
	void Render() const;
	void SetPrimitive(const GLenum primitive);

	void CreateBufferObjects(); // Vertex Object �� Index Object�� �����ϱ� �� �� �Լ��� �ҷ� VAO�� �����Ѵ�.
	void CreateVertexObject(const void* vertices, const GLuint vertexCount, const GLsizeiptr vertexSize);
	void CreateElementObject(const void* indices, const GLuint indexCount, const GLsizeiptr indexSize);

private:
	void ProcessNode(aiNode* node, const aiScene* scene, 
		std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene,
		std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);

private:
	GLuint m_VBO;
	GLuint m_EBO;
	GLuint m_VAO;
	GLenum m_Primitive = GL_TRIANGLES;

	uint16_t m_VertexCount = 0;
	uint16_t m_ElementCount = 0;
};

struct Vertex
{
	Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& tex)
		: m_Position(pos), m_Normal(normal), m_Tangent(tangent), m_Texcoord(tex) { }

	static void SetVertexAttribute()
	{
		// ���� ���� �� �Ӽ� ���Ǹ� �Ͽ��� �Ѵ�.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
		glEnableVertexAttribArray(0); // position

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Normal)));
		glEnableVertexAttribArray(1); // normal

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Tangent)));
		glEnableVertexAttribArray(2); // tangent

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Texcoord)));
		glEnableVertexAttribArray(3); // texcoord

		// ���� ���۵��� �ش� �޽��� VAO�� ������ �ʵ��� ����ε��Ѵ�.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec3 m_Tangent;
	glm::vec2 m_Texcoord;
};

struct LineVertex
{
	LineVertex(const glm::vec3& pos, const glm::vec3& color)
		: m_Position(pos), m_Color(color) { }

	static void SetVertexAttribute()
	{
		// ���� ���� �� �Ӽ� ���Ǹ� �Ͽ��� �Ѵ�.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(0));
		glEnableVertexAttribArray(0); // position

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(offsetof(LineVertex, m_Color)));
		glEnableVertexAttribArray(1); // color

		// ���� ���۵��� �ش� �޽��� VAO�� ������ �ʵ��� ����ε��Ѵ�.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glm::vec3 m_Position;
	glm::vec3 m_Color;
};
