#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"

class Shader;

class Shader
{
public:
	Shader();
	~Shader();

public:
	// ������ �о� �ش�Ǵ� Ÿ���� ���̴��� ���α׷��� �߰��Ѵ�.
	void AddShader(const GLenum shaderType, const char* fileName);
	// ���ϴ� ���̴��� ��� Attack�Ͽ��ٸ� ���α׷��� ��ũ�Ѵ�.
	void Link();
	// ���̴��� ����Ѵ�.
	void Use();

	GLint GetAttribLocation(const char* name) const;
	GLint GetUniformLocation(const char* name) const;
	GLuint GetID() const;

	void SetUniform(const char* name, bool value) const;
	void SetUniform(const char* name, int value) const;
	void SetUniform(const char* name, const float& value, const int count = 1) const;
	void SetUniform(const char* name, const glm::vec2& value, const int count = 1) const;
	void SetUniform(const char* name, const glm::vec3& value, const int count = 1) const;
	void SetUniform(const char* name, const glm::vec4& value, const int count = 1) const;
	void SetUniform(const char* name, const glm::mat3& value, const int count = 1) const;
	void SetUniform(const char* name, const glm::mat4& value, const int count = 1) const;

	void BindTexture(const char* name, const int location, const GLuint texture, GLenum target = GL_TEXTURE_2D) const;

private:
	static bool ReadFile(const char* filename, std::string& target);
	static GLuint CompileShader(const GLenum shaderType, const char* shaderText);

private:
	GLuint m_ShaderProgram;
};