#include "stdafx.h"
#include "Shader.h"

const static std::string shaderPath = "Shaders/";

Shader::Shader()
{
	m_ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (m_ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}
}

Shader::~Shader() { }

void Shader::AddShader(const GLenum shaderType, const char* fileName)
{
	std::string shaderText;
	std::string filePath = shaderPath + fileName;

	bool result = ReadFile(filePath.c_str(), shaderText);
	if (!result) {
		std::cout << filePath.c_str() << " Error compiling shader" << std::endl;
		return;
	};

	GLuint shaderObj = CompileShader(shaderType, shaderText.c_str());
	if (shaderObj == -1)
	{
		return;
	}

	// ShaderProgram 에 attach!!
	glAttachShader(m_ShaderProgram, shaderObj);

	// ShaderPrograme에 링크된 쉐이더는 더이상 필요하지 않음
	glDeleteShader(shaderObj);

	return;
}

bool Shader::ReadFile(const char* filename, std::string& target)
{
	static const std::string includeIndentifier = "#include ";

	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.." << std::endl;
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {

		// include된 파일을 찾아 다시 로드한다.
		if (line.find(includeIndentifier) != line.npos)
		{
			line.erase(0, includeIndentifier.size());
			line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());

			std::string filePath = shaderPath + line;
			ReadFile(filePath.c_str(), target);

			continue;
		}

		target.append(line.c_str());
		target.append("\n");
	}
	return true;
}

GLuint Shader::CompileShader(const GLenum shaderType, const char* shaderText)
{
	//쉐이더 오브젝트 생성
	GLuint shaderObj = glCreateShader(shaderType);

	if (shaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", shaderType);
		return -1;
	}

	const GLchar* p[1];
	GLint Lengths[1];
	p[0] = shaderText;
	Lengths[0] = (GLint)strlen(shaderText);

	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(shaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(shaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, InfoLog);
		printf("%s \n", shaderText);

		return -1;
	}

	return shaderObj;
}

void Shader::Link()
{
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(m_ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(m_ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << "Error linking shader program\n" << ErrorLog;
		return;
	}

	glValidateProgram(m_ShaderProgram);
	glGetProgramiv(m_ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(m_ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << "Error validating shader program\n" << ErrorLog;
		return;
	}
}

void Shader::Use()
{
	glUseProgram(m_ShaderProgram);
}

GLint Shader::GetAttribLocation(const char* name) const
{
	return glGetAttribLocation(m_ShaderProgram, name);
}

GLint Shader::GetUniformLocation(const char* name) const
{
	return glGetUniformLocation(m_ShaderProgram, name);
}

GLuint Shader::GetID() const
{
	return m_ShaderProgram;
}

void Shader::SetUniform(const char* name, bool value) const
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform(const char* name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform(const char* name, const float& value, const int count) const
{
	glUniform1fv(GetUniformLocation(name), count, &value);
}

void Shader::SetUniform(const char* name, const glm::vec2& value, const int count) const
{
	glUniform2fv(GetUniformLocation(name), count, &value[0]);
}

void Shader::SetUniform(const char* name, const glm::vec3& value, const int count) const
{
	glUniform3fv(GetUniformLocation(name), count, &value[0]);
}

void Shader::SetUniform(const char* name, const glm::vec4& value, const int count) const
{
	glUniform4fv(GetUniformLocation(name), count, &value[0]);
}

void Shader::SetUniform(const char* name, const glm::mat3& value, const int count) const
{
	glUniformMatrix3fv(GetUniformLocation(name), count, GL_FALSE, &value[0][0]);
}

void Shader::SetUniform(const char* name, const glm::mat4& value, const int count) const
{
	glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, &value[0][0]);
}

void Shader::BindTexture(const char* name, const int location, const GLuint texture, GLenum target) const
{
	SetUniform(name, location);
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(target, texture);
}