#include "SSAOBuffer.h"

#include "../Shader.h"

SSAOBuffer::SSAOBuffer(int width, int height)
{
    Init(width, height);
}

void SSAOBuffer::Init(int width, int height)
{
    this->m_width = width;
    this->m_height = height;

    CreateFramebuffer();
    Bind();

    glGenTextures(1, &m_ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBuffer, 0);

    CheckComplete();
}

void SSAOBuffer::BindTextures(Shader& shader) const
{
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
    shader.setInt("ssao", 4);
}
