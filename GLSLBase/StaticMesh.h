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
	// 파일을 불러오지 않을 경우 버텍스 및 인덱스를 직접 정의한다.
	StaticMesh();
	// 파일을 불러올 경우 Create함수를 불러올 필요는 없다.
	StaticMesh(const char* fileName);
	~StaticMesh();

public:
	void Render() const;
	void SetPrimitive(const GLenum primitive);

	void CreateBufferObjects(); // Vertex Object 및 Index Object를 생성하기 전 이 함수를 불러 VAO를 설정한다.
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
		// 버퍼 생성 후 속성 정의를 하여야 한다.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
		glEnableVertexAttribArray(0); // position

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Normal)));
		glEnableVertexAttribArray(1); // normal

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Tangent)));
		glEnableVertexAttribArray(2); // tangent

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Texcoord)));
		glEnableVertexAttribArray(3); // texcoord

		// 이후 버퍼들이 해당 메쉬의 VAO에 섞이지 않도록 언바인딩한다.
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
		// 버퍼 생성 후 속성 정의를 하여야 한다.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(0));
		glEnableVertexAttribArray(0); // position

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(offsetof(LineVertex, m_Color)));
		glEnableVertexAttribArray(1); // color

		// 이후 버퍼들이 해당 메쉬의 VAO에 섞이지 않도록 언바인딩한다.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glm::vec3 m_Position;
	glm::vec3 m_Color;
};
