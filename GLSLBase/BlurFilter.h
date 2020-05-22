#pragma once

#include <vector>

#include "Dependencies\glew.h"
#include "UniformBuffer.hpp"

#define MAX_RADIUS 5

class Shader;

class BlurFilter
{
public:
	BlurFilter(const int width, const int height);
	~BlurFilter();

public:
	void Execute(const GLuint texture, const int blurCount) const;
	GLuint GetOutput() const;

private:
	void CreateBuffers();
	std::vector<float> CalcGaussWeights(const float sigma) const;

private:
	int m_Width;
	int m_Height;

	std::vector<float> m_Weights;
	int m_BlurRadius;

	GLuint m_FBO;
	GLuint m_BlurBuffer;
	GLuint m_Output;
};