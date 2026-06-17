#include "SSAOBlurBuffer.h"

#include"../Shader.h"

SSAOBlurBuffer::SSAOBlurBuffer(int width, int height)
{
    Init(width, height);
}

void SSAOBlurBuffer::Init(int width, int height)
{
    this->m_width = width;
    this->m_height = height;

    CreateFramebuffer();
    Bind();

    glGenTextures(1, &m_ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBufferBlur, 0);

    CheckComplete();

    GenerateNoiseTexture();
}

void SSAOBlurBuffer::GenerateNoiseTexture()
{
    // Generate the sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    m_ssaoKernel.clear();
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssaoKernel.push_back(sample);
    }

    // Generate the noise texture

    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &m_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glCheckError();
}

float SSAOBlurBuffer::Lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void SSAOBlurBuffer::BindTextures(Shader& shader) const
{
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);

    shader.setInt("ssao", 4);
}

void SSAOBlurBuffer::DestroyAttachments()
{
    if (m_noiseTexture != 0)
    {
        glDeleteTextures(1, &m_noiseTexture);
    }
}
