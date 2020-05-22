#include "stdafx.h"
#include "BlurFilter.h"
#include "Shader.h"
#include "Renderer.h"

BlurFilter::BlurFilter(const int width, const int height)
{
	// A good value for 9x9 is around 3 to 5
	// A good value for 7x7 is around 2.5 to 4
	// A good value for 5x5 is around 2 to 3.5

	m_Width = width;
	m_Height = height;
	m_Weights = CalcGaussWeights(2.5f);
	m_BlurRadius = (int)m_Weights.size() / 2;

	CreateBuffers();
}

BlurFilter::~BlurFilter() 
{
	glDeleteTextures(1, &m_BlurBuffer);
	glDeleteTextures(1, &m_Output);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_FBO);
}

void BlurFilter::Execute(const GLuint texture, const int blurCount) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glDrawBuffers(1, Renderer::attachments_OneColorBuffer);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	Shader* blurShader = Renderer::GetInstance()->GetShader("Blur");
	blurShader->Use();

	blurShader->SetUniform("g_BlurRadius", m_BlurRadius);
	blurShader->SetUniform("g_Weights", m_Weights[m_BlurRadius], m_BlurRadius);

	glViewport(0, 0, m_Width, m_Height);

	for (int i = 0; i < blurCount; ++i)
	{
		if (i == 0)
		{
			// Horizontal Blur
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurBuffer, 0);
			blurShader->SetUniform("g_Horizontal", true);
			blurShader->BindTexture("g_Texture", 0, texture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else
		{
			// Horizontal Blur
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurBuffer, 0);
			blurShader->SetUniform("g_Horizontal", true);
			blurShader->BindTexture("g_Texture", 0, m_Output);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// Vertical Blur
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Output, 0);
		blurShader->SetUniform("g_Horizontal", false);
		blurShader->BindTexture("g_Texture", 0, m_BlurBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlurFilter::CreateBuffers()
{
	glGenTextures(1, &m_BlurBuffer);
	glBindTexture(GL_TEXTURE_2D, m_BlurBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_Output);
	glBindTexture(GL_TEXTURE_2D, m_Output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 프레임 버퍼 생성
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// 언바인딩 = 디폴트 프레임 버퍼로 변경
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::vector<float> BlurFilter::CalcGaussWeights(const float sigma) const
{
	float twoSigma2 = 2.0f * sigma * sigma;
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius <= MAX_RADIUS);

	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;

	for (int i = -blurRadius; i <= blurRadius; ++i)
	{
		float x = (float)i;

		weights[i + blurRadius] = expf(-x * x / twoSigma2);

		weightSum += weights[i + blurRadius];
	}

	for (int i = 0; i < weights.size(); ++i)
	{
		weights[i] /= weightSum;
	}

	return weights;
}

GLuint BlurFilter::GetOutput() const
{
	return m_Output;
}