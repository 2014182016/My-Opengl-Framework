#pragma once

#include "Dependencies\glew.h"
#include "Dependencies\glm/glm.hpp"

class Shader;

struct Material
{
	// ½¦ÀÌ´õ¿¡ ¹­À» uniform º¯¼ö¸¦ ¿©±â¼­ ¹­´Â´Ù.
	void Bind(Shader* shader) const;

	glm::vec4 m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 m_Specular = glm::vec3(0.1f, 0.1f, 0.1f);
	float m_Roughness = 0.1f;
	glm::vec3 m_Emissive = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_Metallic = 0.9f;

	GLuint m_DiffuseMap = 0;
	GLuint m_NormalMap = 0;
	GLuint m_SpecularMap = 0;
	GLuint m_RoughnessMap = 0;
	GLuint m_MetallicMap = 0;
	GLuint m_AOMap = 0;
};

