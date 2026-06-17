#include "Framebuffer.h"

#include "../Shader.h"

Framebuffer::Framebuffer(int width, int height)
	: m_width(width),
	m_height(height)
{

}

Framebuffer::~Framebuffer()
{
	if (m_fbo != 0)
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}

	m_width = 0;
	m_height = 0;
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
{
	if (this == &other)
	{
		return;
	}

	m_fbo = other.m_fbo;
	m_width = other.m_width;
	m_height = other.m_height;

}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	Destroy();

	m_fbo = other.m_fbo;
	m_width = other.m_width;
	m_height = other.m_height;


	return *this;
}

void Framebuffer::CreateFramebuffer()
{
	if (m_fbo == 0)
	{
		glGenFramebuffers(1, &m_fbo);
	}
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int newWidth, int newHeight)
{
	if (newWidth == m_width && newHeight == m_height)
	{
		return;
	}
}

void Framebuffer::Destroy()
{
	DestroyAttachments();

	if (m_fbo != 0)
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}

	m_width = 0;
	m_height = 0;
}

void Framebuffer::BindGenericTexture(GLuint texture, const std::string& name, Shader& shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInt(name, 0);
}

void Framebuffer::CheckComplete() const
{
	Bind();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete!\n";
	}
}
