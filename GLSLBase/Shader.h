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
	// 파일을 읽어 해당되는 타입의 쉐이더를 프로그램에 추가한다.
	void AddShader(const GLenum shaderType, const char* fileName);
	// 원하는 쉐이더를 모두 Attack하였다면 프로그램에 링크한다.
	void Link();
	// 쉐이더를 사용한다.
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