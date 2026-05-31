#include "DebugUtils.h"

class GBuffer
{
    GLuint fbo = 0;

    GLuint gPosition = 0;
    GLuint gNormal = 0;
    GLuint gAlbedoSpec = 0;
    GLuint depthRBO = 0;

    int width = 0;
    int height = 0;

    void Init(int width, int height);
    void BindForWriting();
    void BindForReading(Shader& shader);
    void Resize(int width, int height);
};