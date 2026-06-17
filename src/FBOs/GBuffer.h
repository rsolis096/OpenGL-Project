#pragma once

#include "Framebuffer.h"

class Shader;

class GBuffer : public Framebuffer
{
public:
    GBuffer(int width, int height);
    ~GBuffer() override;

    void Init(int width, int height);

    // Overridden methods
    void BindTextures(Shader& shader) const override;
    void DestroyAttachments() override;
    void Resize(int newWidth, int newHeight) override;

    GLuint GetPositionTexture() const { return m_gPosition; }
    GLuint GetNormalTexture() const { return m_gNormal; }
    GLuint GetAlbedoSpecTexture() const { return m_gAlbedoSpec; }

private:
    static void SetGBufferTextureParams();
private:

    // Textures
    GLuint m_gPosition = 0;
    GLuint m_gNormal = 0;
    GLuint m_gAlbedoSpec = 0;

    GLuint m_depthRBO = 0;
};