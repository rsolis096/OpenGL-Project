#pragma once

#include <glad/glad.h>

class Shader;

class GBuffer
{
public:
    GBuffer(int width, int height);
    ~GBuffer();

    void Init(int width, int height);
    void BindForWriting();
    void BindForReading(Shader& shader) const;
    void Resize(int width, int height);
    void Destroy();

    GLuint GetPositionTexture() const { return gPosition; }
    GLuint GetNormalTexture() const { return gNormal; }
    GLuint GetAlbedoSpecTexture() const { return gAlbedoSpec; }
    GLuint GetFBO() const { return gBufferFBO; }

private:
    static void SetGBufferTextureParams();

private:
    GLuint gBufferFBO = 0;

    GLuint gPosition = 0;
    GLuint gNormal = 0;
    GLuint gAlbedoSpec = 0;

    GLuint depthRBO = 0;

    int width = 0;
    int height = 0;
};