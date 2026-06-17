#include "Framebuffer.h"

class SSAOBuffer : public Framebuffer
{
public:
    SSAOBuffer(int width, int height);
    void BindTextures(Shader& shader) const override;

    GLuint GetColorBuffer() const { return m_ssaoColorBuffer; };


private:

    void Init(int width, int height);

    GLuint m_ssaoColorBuffer;
};