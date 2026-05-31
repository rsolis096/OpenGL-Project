#include "GBuffer.h"

#include "Shader.h"

#include <iostream>

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
    this->width = width;
    this->height = height;

    glGenFramebuffers(1, &gBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

    // Position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Normal
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Albedo + specular
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    SetGBufferTextureParams();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    GLuint attachments[3] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);

    // Depth
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "GBuffer framebuffer is not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::SetGBufferTextureParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
}

void GBuffer::BindForReading(Shader& shader) const
{
    shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    shader.setInt("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    shader.setInt("gNormal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    shader.setInt("gAlbedoSpec", 2);
}

void GBuffer::Resize(int newWidth, int newHeight)
{
    if (newWidth == width && newHeight == height)
        return;

    Destroy();
    Init(newWidth, newHeight);
}

void GBuffer::Destroy()
{
    if (gPosition != 0)
        glDeleteTextures(1, &gPosition);

    if (gNormal != 0)
        glDeleteTextures(1, &gNormal);

    if (gAlbedoSpec != 0)
        glDeleteTextures(1, &gAlbedoSpec);

    if (depthRBO != 0)
        glDeleteRenderbuffers(1, &depthRBO);

    if (gBufferFBO != 0)
        glDeleteFramebuffers(1, &gBufferFBO);

    gPosition = 0;
    gNormal = 0;
    gAlbedoSpec = 0;
    depthRBO = 0;
    gBufferFBO = 0;
    width = 0;
    height = 0;
}