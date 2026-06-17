#include <random>
#include "Framebuffer.h"

class SSAOBlurBuffer : public Framebuffer
{
public:
    SSAOBlurBuffer(int width, int height);
    void BindTextures(Shader& shader) const override;
    void DestroyAttachments() override;

    GLuint GetNoiseTexture() const { return m_noiseTexture; };
    GLuint GetColorBuffer() const { return m_ssaoColorBufferBlur; };
    const std::vector<glm::vec3>& GetSSAOKernel() const { return m_ssaoKernel; };

private:

    void Init(int width, int height);

    void GenerateNoiseTexture();

    float Lerp(float a, float b, float f);

    GLuint m_ssaoColorBufferBlur;

    GLuint m_noiseTexture;

    std::vector<glm::vec3> m_ssaoKernel;

};