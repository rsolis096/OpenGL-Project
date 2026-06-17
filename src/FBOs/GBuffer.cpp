#include "GBuffer.h"

#include "../Shader.h"

GBuffer::GBuffer(int w, int h)
{
    Init(w, h);
}

GBuffer::~GBuffer()
{
    Destroy();
}

void GBuffer::Init(int width, int height)
{
    this->m_width = width;
    this->m_height = height;

    CreateFramebuffer();
    Bind();

    // Position
    glGenTextures(1, &m_gPosition);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

    // Normal
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

    // Albedo + specular
    glGenTextures(1, &m_gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

    GLuint attachments[3] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);

    // Depth
    glGenRenderbuffers(1, &m_depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRBO);

    CheckComplete();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::SetGBufferTextureParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GBuffer::BindTextures(Shader& shader) const
{
    shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    shader.setInt("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    shader.setInt("gNormal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    shader.setInt("gAlbedoSpec", 2);
}

void GBuffer::Resize(int newWidth, int newHeight)
{
    if (newWidth == m_width && newHeight == m_height)
    {
        return;
    }
    
    Init(newWidth, newHeight);
}

void GBuffer::DestroyAttachments()
{
    if (m_gPosition != 0)
        glDeleteTextures(1, &m_gPosition);

    if (m_gNormal != 0)
        glDeleteTextures(1, &m_gNormal);

    if (m_gAlbedoSpec != 0)
        glDeleteTextures(1, &m_gAlbedoSpec);

    if (m_depthRBO != 0)
        glDeleteRenderbuffers(1, &m_depthRBO);

    m_gPosition = 0;
    m_gNormal = 0;
    m_gAlbedoSpec = 0;
    m_depthRBO = 0;
}