#include "stdafx.h"
#include "StaticMesh.h"
#include "Dependencies\assimp\include\assimp/Importer.hpp"
#include "Dependencies\assimp\include\assimp/scene.h"
#include "Dependencies\assimp\include\assimp/postprocess.h"

StaticMesh::StaticMesh() { }

StaticMesh::StaticMesh(const char* fileName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath + fileName,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	ProcessNode(scene->mRootNode, scene, vertices, indices);

	CreateBufferObjects();
	CreateVertexObject(vertices.data(), (GLuint)vertices.size(), sizeof(Vertex));
	CreateElementObject(indices.data(), (GLuint)indices.size(), sizeof(uint16_t));
	Vertex::SetVertexAttribute();
}


StaticMesh::~StaticMesh()
{
	// 버퍼를 지운다.
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void StaticMesh::ProcessNode(aiNode* node, const aiScene* scene, 
	std::vector<Vertex>& vertices, std::vector<uint16_t>& indices)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, vertices, indices);
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, vertices, indices);
	}
}

void StaticMesh::ProcessMesh(aiMesh* mesh, const aiScene* scene,
	std::vector<Vertex>& vertices, std::vector<uint16_t>& indices)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		glm::vec3 tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		glm::vec2 texcoord = glm::vec2();
		if (mesh->mTextureCoords[0])
			texcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

		vertices.emplace_back(pos, normal, tangent, texcoord);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.emplace_back(face.mIndices[j]);
	}
}

void StaticMesh::CreateBufferObjects()
{
	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO); // just generate ID
	glGenBuffers(1, &m_EBO); // just generate ID

	glBindVertexArray(m_VAO);
}

void StaticMesh::CreateVertexObject(const void* vertices, const GLuint vertexCount, const GLsizeiptr vertexSize)
{
	m_VertexCount = (uint16_t)vertexCount;

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); 
	glBufferData(GL_ARRAY_BUFFER, vertexSize * m_VertexCount, vertices, GL_STATIC_DRAW);
}

void StaticMesh::CreateElementObject(const void* indices, const GLuint indexCount, const GLsizeiptr indexSize)
{
	m_ElementCount = (uint16_t)indexCount;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_ElementCount, indices, GL_STATIC_DRAW);
}

void StaticMesh::Render() const
{
	if (m_VertexCount == 0)
	{
		std::cout << "버텍스 버퍼를 생성하지 않음" << std::endl;
		return;
	}

	// 미리 셋팅된 VAO를 바인딩한다.
	glBindVertexArray(m_VAO);

	// Element Object를 사용하여 그린다.
	if (m_ElementCount != 0)
	{
		glDrawElements(m_Primitive, m_ElementCount, GL_UNSIGNED_SHORT, 0);
	}
	// Vertex Object만 사용하여 그린다.
	else
	{
		glDrawArrays(m_Primitive, 0, m_VertexCount);
	}
	
	// 지정한 VAO를 Unbind한다.
	glBindVertexArray(0);
}

void StaticMesh::SetPrimitive(const GLenum primitive)
{
	m_Primitive = primitive;
}
